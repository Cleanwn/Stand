#include "HttpRequestBuilder.hpp"

#include <fmt/format.h>

#include <soup/dnsHttpResolver.hpp>
#include <soup/dnsOsResolver.hpp>
#include <soup/dnsSmartResolver.hpp>
#include <soup/dnsUdpResolver.hpp>
#include <soup/HttpRequestTask.hpp>
#include <soup/HttpResponse.hpp>
#include <soup/netConfig.hpp>
#include <soup/netStatus.hpp>
#include <soup/ObfusString.hpp>
#include <soup/Socket.hpp>
#include <soup/X509Certchain.hpp>

#include "Exceptional.hpp"
#include "lang.hpp"
#include "LangId.hpp"
#include "NetInterface.hpp"
#include "Util.hpp"

// Intended to reduce load on DoH server, but our usage patterns + keep-alive + low TTL has us missing the cache too much.
#define USE_DNS_CACHE false

#if USE_DNS_CACHE
#include <soup/dnsCacheResolver.hpp>
#endif

namespace Stand
{
	std::string HttpRequestBuilder::method_str(Method method)
	{
		return (method == GET ? soup::ObfusString("GET") : soup::ObfusString("POST"));
	}

	HttpRequestBuilder::HttpRequestBuilder(Method method, std::string&& host, std::string&& path)
		: HttpRequest(method_str(method), std::move(host), std::move(path))
	{
	}

	std::pair<std::string, std::string> HttpRequestBuilder::parseHostPathString(const std::string& host_and_path)
	{
		auto sep = host_and_path.find('/');
		std::string host{};
		std::string path{};
		if (sep == std::string::npos)
		{
			host = host_and_path;
		}
		else
		{
			host = host_and_path.substr(0, sep);
			path = host_and_path.substr(sep);
		}
		return std::pair<std::string, std::string>(std::move(host), std::move(path));
	}

	HttpRequestBuilder HttpRequestBuilder::fromHostPathString(Method method, const std::string& host_and_path)
	{
		auto [host, path] = parseHostPathString(host_and_path);
		return HttpRequestBuilder(method, std::move(host), std::move(path));
	}

	void HttpRequestBuilder::setPathWithRockstarLocalisation(std::string&& path) noexcept
	{
		if (Lang::active_id != LANG_EN && Lang::isSupportedByRockstar(Lang::active_id))
		{
			path.insert(0, Lang::idToCodeForRockstar(Lang::active_id));
			path.insert(0, 1, '/');
		}
		setPath(std::move(path));
	}

	struct HttpRequestTask : public soup::Task
	{
		soup::HttpRequestTask subtask;
		std::function<void(soup::HttpResponse&&)> response_callback;
		std::function<void(std::string&&)> fail_callback;

		HttpRequestTask(HttpRequestBuilder&& b)
			: subtask(std::move(static_cast<soup::HttpRequest&>(b))), response_callback(std::move(b.response_callback)), fail_callback(std::move(b.fail_callback))
		{
			subtask.prefer_ipv6 = b.prefer_ipv6;
		}

		void onTick() final
		{
			if (subtask.tickUntilDone())
			{
				if (subtask.result.has_value())
				{
					response_callback(std::move(*subtask.result));
				}
				else
				{
					fail_callback(subtask.getStatus());
				}
				setWorkDone();
			}
		}

		[[nodiscard]] int getSchedulingDisposition() const noexcept final
		{
			return subtask.getSchedulingDisposition();
		}

		[[nodiscard]] std::string toString() const SOUP_EXCAL final
		{
			return subtask.toString();
		}
	};

	void HttpRequestBuilder::dispatch() noexcept
	{
		g_net_interface.add<HttpRequestTask>(std::move(*this));
	}

	void HttpRequestBuilder::updateNetConfig(soup::netConfig& conf) SOUP_EXCAL
	{
		soup::UniquePtr<soup::dnsResolver> resolver;
		switch (dns_conduit)
		{
		default:
		case Stand::DNS_CONDUIT_OS:
			resolver = soup::make_unique<soup::dnsOsResolver>();
			break;
		case Stand::DNS_CONDUIT_SMART:
			resolver = soup::make_unique<soup::dnsSmartResolver>();
			break;
		case Stand::DNS_CONDUIT_HTTP:
			resolver = soup::make_unique<soup::dnsHttpResolver>();
			break;
		case Stand::DNS_CONDUIT_UDP:
			resolver = soup::make_unique<soup::dnsUdpResolver>();
			break;
		}
#if USE_DNS_CACHE
		conf.dns_resolver = soup::make_unique<soup::dnsCacheResolver>(std::move(resolver));
#else
		conf.dns_resolver = std::move(resolver);
#endif

		conf.certchain_validator = [](const soup::X509Certchain& chain, const std::string& name, soup::StructMap& custom_data) SOUP_EXCAL
		{
			if (soup::Socket::certchain_validator_default(chain, name, custom_data))
			{
				if (rage::atStringHash(name) != ATSTRINGHASH("stand.sh"))
				{
					return true;
				}
				if (chain.certs.size() >= 2
					&& chain.certs.at(0).isEc())
				{
					return true;
				}
			}
			Util::toast(fmt::format(fmt::runtime(soup::ObfusString("Connection Error: Certificate for {} failed validation. (Chain: {})").str()), name, chain.toString()), TOAST_ALL);
			return false;
		};
	}
}
