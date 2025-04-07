#include <iostream>

#include <HttpRequest.hpp>
#include <netConfig.hpp>
#include <pem.hpp>
#include <Promise.hpp>
#include <Scheduler.hpp>
#include <Server.hpp>
#include <ServerWebService.hpp>
#include <Socket.hpp>
#include <string.hpp>
#include <TlsServerRsaData.hpp>
#include <WebSocketMessage.hpp>

struct RelayClientData
{
	bool is_mod = false;
	std::string key{};
	std::string line_buf{};
};

static soup::Server serv{};

[[nodiscard]] static soup::Socket* getSocket(bool mod, const std::string& key)
{
	for (const auto& w : serv.workers)
	{
		auto s = reinterpret_cast<soup::Socket*>(w.get());
		RelayClientData& cd = s->custom_data.getStructFromMap(RelayClientData);
		if (cd.is_mod == mod
			&& cd.key == key
			)
		{
			return s;
		}
	}
	return nullptr;
}

[[nodiscard]] static soup::Socket* getMod(const std::string& key)
{
	return getSocket(true, key);
}

[[nodiscard]] static soup::Socket* getWeb(const std::string& key)
{
	return getSocket(false, key);
}

static void handleDisconnect(soup::Socket& s)
{
	RelayClientData& cd = s.custom_data.getStructFromMap(RelayClientData);
	if (!cd.key.empty())
	{
		if (cd.is_mod)
		{
			if (auto ws = getWeb(cd.key))
			{
				soup::ServerWebService::wsSend(*ws, "lost_mod", true);
			}
		}
		else
		{
			if (auto ms = getMod(cd.key))
			{
				ms->send("lost_web\n");
			}
		}
	}
}

static void modRecvLoop(soup::Socket& s) SOUP_EXCAL
{
	s.recv([](soup::Socket& s, std::string&& data, soup::Capture&&) SOUP_EXCAL
	{
		RelayClientData& cd = s.custom_data.getStructFromMap(RelayClientData);
		if (!cd.key.empty())
		{
			cd.line_buf.append(data);
			auto off = cd.line_buf.find('\n');
			if (off != std::string::npos)
			{
				if (auto ws = getWeb(cd.key))
				{
					do
					{
						soup::ServerWebService::wsSend(*ws, cd.line_buf.substr(0, off), true);
						cd.line_buf.erase(0, off + 1);
					} while (off = cd.line_buf.find('\n'), off != std::string::npos);
				}
			}
			modRecvLoop(s);
		}
	});
}

static soup::TlsServerRsaData server_rsa_data;

static void cert_selector(soup::TlsServerRsaData& out, const std::string& server_name) SOUP_EXCAL
{
	out = server_rsa_data;
}

int main()
{
	server_rsa_data.der_encoded_certchain = soup::pem::decodeChain(soup::string::fromFile("cert/certplusissuer.pem"));
	server_rsa_data.private_key = soup::RsaPrivateKey::fromPem(soup::string::fromFile("cert/privkey.pem"));

	serv.on_work_done = [](soup::Worker& w, soup::Scheduler&)
	{
		soup::Socket& s = reinterpret_cast<soup::Socket&>(w);
		std::cout << s.peer.toString() << " - work done" << std::endl;
		handleDisconnect(s);
	};
	serv.on_connection_lost = [](soup::Socket& s, soup::Scheduler&)
	{
		std::cout << s.peer.toString() << " - connection lost" << std::endl;
		handleDisconnect(s);
	};
	serv.on_exception = [](soup::Worker& w, const std::exception& e, soup::Scheduler&)
	{
		soup::Socket& s = reinterpret_cast<soup::Socket&>(w);
		std::cout << s.peer.toString() << " - exception: " << e.what() << std::endl;
		handleDisconnect(s);
	};

	soup::ServerService stand_srv{
		[](soup::Socket& s, soup::ServerService&, soup::Server&) SOUP_EXCAL
		{
			std::cout << s.peer.toString() << " + mod connection established" << std::endl;
			s.send("Go ahead\n");
			s.recv([](soup::Socket& s, std::string&& data, soup::Capture&&) SOUP_EXCAL
			{
				std::cout << s.peer.toString() << " > " << data << std::endl;
				if (data.substr(0, 6) == "Stand:")
				{
					if (data.substr(6, 2) != "1:")
					{
						s.send("notify_above_map Your version of Stand is not supported by this relay.\n");
					}
					else
					{
						std::string key = data.substr(8);
						if (auto ms = getMod(key))
						{
							ms->custom_data.getStructFromMap(RelayClientData).key.clear();
							ms->close();
						}
						RelayClientData& cd = s.custom_data.getStructFromMap(RelayClientData);
						cd.is_mod = true;
						cd.key = key;
						auto p = soup::make_unique<soup::Promise<bool>>([](soup::Capture&& cap) -> bool
						{
							// This takes too long if we actually validate the cert
							soup::netConfig::get().certchain_validator = &soup::Socket::certchain_validator_none;

							std::string path = "internal_get_privilege/?0=";
							path.append(cap.get<std::string>());
							soup::HttpRequest req{ "backend-services.stand.sh", std::move(path) };
							auto res = req.execute();
							if (!res.has_value())
							{
								return false;
							}
							return soup::string::toInt<uint8_t>(res->body) >= 2;
						}, std::move(key));
						soup::PromiseBase* pp = p.get();
						s.awaitPromiseCompletion(pp, [](soup::Worker& w, soup::Capture&& cap)
						{
							auto& p = cap.get<soup::UniquePtr<soup::Promise<bool>>>();
							if (p.get() && p->getResult())
							{
								soup::Socket& s = reinterpret_cast<soup::Socket&>(w);
								if (auto ws = getWeb(s.custom_data.getStructFromMap(RelayClientData).key))
								{
									s.send("got_web\n");
								}
								modRecvLoop(s);
							}
						}, std::move(p));
					}
				}
			});
		}
	};

	soup::ServerWebService web_srv{
		[](soup::Socket& s, soup::HttpRequest&& req, soup::ServerWebService&)
		{
			soup::ServerWebService::disableKeepAlive(s);
			soup::ServerWebService::sendText(s, ":)");
		}
	};
	web_srv.on_connection_established = [](soup::Socket& s, soup::ServerService&, soup::Server&) SOUP_EXCAL
	{
		std::cout << s.peer.toString() << " + web connection established" << std::endl;
	};
	web_srv.should_accept_websocket_connection = [](soup::Socket& s, const soup::HttpRequest& req, soup::ServerWebService&) SOUP_EXCAL
	{
		std::cout << s.peer.toString() << " > WEBSOCKET " << req.path << std::endl;
		return true;
	};
	web_srv.on_websocket_message = [](soup::WebSocketMessage& msg, soup::Socket& s, soup::ServerWebService&)
	{
		RelayClientData& cd = s.custom_data.getStructFromMap(RelayClientData);
		if (cd.key.empty())
		{
			if (msg.data.substr(0, 8) == "license ")
			{
				auto key = msg.data.substr(8);
				if (auto ws = getWeb(key))
				{
					ws->custom_data.getStructFromMap(RelayClientData).key.clear();
					ws->close();
				}
				cd.key = std::move(key);
				if (auto ms = getMod(cd.key))
				{
					ms->send("got_web\n");
				}
				else
				{
					soup::ServerWebService::wsSend(s, "no_mod", true);
				}
				return;
			}
		}
		else
		{
			if (msg.data.substr(0, 2) == "p "
				|| msg.data.substr(0, 2) == "k "
				|| msg.data.substr(0, 2) == "c "
				|| msg.data.substr(0, 2) == "s "
				)
			{
				if (auto ms = getMod(cd.key))
				{
					msg.data.push_back('\n');
					ms->send(std::move(msg.data));
					return;
				}
			}
		}
		s.close();
	};

	if (!serv.bind(25769, &stand_srv))
	{
		std::cout << "Failed to bind to port 25769." << std::endl;
		return 1;
	}
	if (!serv.bindCrypto(4269, &web_srv, &cert_selector))
	{
		std::cout << "Failed to bind to port 4269." << std::endl;
		return 2;
	}
	std::cout << "Listening on ports 25769 and 4269." << std::endl;
	serv.run();
}
