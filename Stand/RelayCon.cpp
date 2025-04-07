#include "RelayCon.hpp"

#include <fmt/core.h>

#include <soup/ObfusString.hpp>
#include <soup/Socket.hpp>

#include "Auth.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

#define RELAY_PORT 25769

namespace Stand
{
	bool RelayCon::isRunning() const noexcept
	{
		return thread_running;
	}

	void RelayCon::setServerAndInit()
	{
		g_relay.server = soup::ObfusString("relay1.stand.sh").str();
		g_relay.init();
	}

	void RelayCon::init()
	{
		if (!thread_running)
		{
			thread_running = true;
			thread = Exceptional::createExceptionalThread([]
			{
				THREAD_NAME("RelayCon");
				g_relay.run();
				g_relay.thread_running = false;
				CloseHandle(g_relay.thread);
			});
		}
	}

	void RelayCon::run()
	{
		while (true)
		{
			Util::toast(LANG_FMT("RELAY_C", server), TOAST_ABOVE_MAP);

			soup::Socket sock{};
			if (!sock.connect(server, RELAY_PORT))
			{
				Util::toast(LANG_FMT("RELAY_E", errorCodeToString(getLastError())), TOAST_ABOVE_MAP);
				break;
			}
			sock.setNonBlocking();
			setRecvHandler(*TcpConnection::addSocket(std::move(sock)));
			TcpConnection::run();

			const bool was_using_web_interface = (g_gui.web_focus != nullptr);

			Util::toast(LOC("RELAY_L"), TOAST_ABOVE_MAP);

			close();
			recv_message_buffer.clear();
			cleanup();

			if (!was_using_web_interface
				|| g_gui.isUnloadPending()
				)
			{
				break;
			}
		}
	}

	void RelayCon::setRecvHandler(soup::Socket& s) SOUP_EXCAL
	{
		s.recv([](soup::Socket& s, std::string&& data, soup::Capture&& _cap) SOUP_EXCAL
		{
			auto cap = _cap.get<RelayCon*>();
			cap->setRecvHandler(s);
			cap->recv_message_buffer.append(data);
			size_t del_pos;
			while ((del_pos = cap->recv_message_buffer.find('\n')) != std::string::npos)
			{
				cap->processMessage(cap->recv_message_buffer.substr(0, del_pos));
				cap->recv_message_buffer.erase(0, del_pos + 1);
			}
		}, this);
	}

	static void onGotWeb() noexcept
	{
		Exceptional::createManagedExceptionalThread(__FUNCTION__, []
		{
			EXCEPTIONAL_LOCK(g_relay.send_mtx)
			g_relay.sendLang();
			g_gui.sendRootListToWeb();
			EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
		});
	}

	void RelayCon::processMessage(const std::string& message) SOUP_EXCAL
	{
		if (g_gui.isUnloadPending())
		{
			return;
		}
		//g_logger.log(message);
		if (message == "Go ahead")
		{
			sendHandshakeResponse();
		}
		else if (message == "got_web")
		{
			Util::toast(LOC("WEB_T"), TOAST_ABOVE_MAP);
			cleanup();
			/*if (g_gui.m_root_state < GUI_REGULAR)
			{
				FiberPool::queueJob([]
				{
					while (g_gui.m_root_state < GUI_REGULAR)
					{
						script::get_current()->yield();
					}
					onGotWeb();
				});
			}
			else*/
			{
				onGotWeb();
			}
		}
		else if (message == "lost_web")
		{
			if (!g_gui.isUnloadPending())
			{
				Util::toast(LOC("WEB_T_L"), TOAST_ABOVE_MAP);
			}
			cleanup();
		}
		else
		{
			const auto& message_substr_0_2 = message.substr(0, 2);
			if (message_substr_0_2 == "k ")
			{
				Command* const command = getCommand(message.substr(2));
				if (command != nullptr)
				{
					FiberPool::queueJob([command]
					{
						Click click(CLICK_WEB, TC_SCRIPT_YIELDABLE);
						command->getPhysical()->onClick(click);
					});
				}
			}
			else if (message_substr_0_2 == "p ")
			{
				Command* const command = g_gui.root_list->resolveCommandWeb(message.substr(2));
				if (command == nullptr)
				{
					sendLine(std::move(std::string("toast Failed to resolve ").append(message.substr(2))));
				}
				else
				{
					Click click(CLICK_WEB, TC_OTHER);
					command->getPhysical()->onClick(click);
				}
			}
			else if (message_substr_0_2 == "c ")
			{
				std::string data = message.substr(2);
				FiberPool::queueJob([data{ std::move(data) }]() mutable
				{
					Click click(CLICK_WEB_COMMAND, TC_OTHER);
					g_gui.triggerCommands(std::move(data), click);
				});
			}
			else if (message_substr_0_2 == "s ")
			{
				size_t i = message.find(':');
				if (i != std::string::npos)
				{
					Command* const command = getCommand(message.substr(2, i - 2));
					if (command != nullptr)
					{
						std::string data = message.substr(i + 1);
						FiberPool::queueJob([command, data{ std::move(data) }]
						{
							Click click(CLICK_WEB, TC_SCRIPT_YIELDABLE);
							command->getPhysical()->setState(click, data);
						});
					}
				}
			}
			else if (message.length() > 17)
			{
				if (message.substr(0, 17) == "notify_above_map ")
				{
					Util::toast(message.substr(17), TOAST_DEFAULT);
				}
			}
		}
	}

	void RelayCon::sendHandshakeResponse()
	{
		if (g_auth.license_permissions >= LICPERM_REGULAR)
		{
			std::string hello = soup::ObfusString("Stand:1:").str();
			hello.append(g_auth.activation_key_to_try);
			sendRaw(std::move(hello));
		}
		else
		{
			close();
		}
	}

	Command* RelayCon::getCommand(const std::string& target)
	{
		Command* const command = (g_gui.web_focus == nullptr ? g_gui.root_list.get() : g_gui.web_focus)->resolveChildByMenuNameWebString(target);
		if (command == nullptr)
		{
			sendLine(std::move(std::string("toast Unknown command: ").append(target)));
		}
		return command;
	}

	void RelayCon::sendLine(std::string&& str)
	{
		str.push_back('\n');
		EXCEPTIONAL_LOCK(send_mtx)
		sendRaw(std::move(str));
		EXCEPTIONAL_UNLOCK(send_mtx)
	}

	void RelayCon::sendLineAsync(std::string&& str)
	{
		Exceptional::createManagedExceptionalThread(__FUNCTION__, [this, str{ std::move(str) }]() mutable
		{
			sendLine(std::move(str));
		});
	}

	void RelayCon::sendLang()
	{
		auto web_lang = Lang::active_id;
		if (!Lang::isSupportedByWebInterface(web_lang))
		{
			web_lang = LANG_EN;
		}
		sendLine(std::move(std::string("lang ").append(Lang::idToCodeA(web_lang))));
	}

	void RelayCon::cleanup() noexcept
	{
		g_gui.web_focus = nullptr;
	}
}
