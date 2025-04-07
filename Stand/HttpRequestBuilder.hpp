#pragma once

#include <cstdint>

#include <soup/HttpRequest.hpp>

#include "nullsub.hpp"

namespace Stand
{
	enum DnsConduit : uint8_t
	{
		DNS_CONDUIT_OS = 0,
		DNS_CONDUIT_SMART,
		DNS_CONDUIT_HTTP,
		DNS_CONDUIT_UDP,
	};

#pragma pack(push, 1)
	class HttpRequestBuilder : public soup::HttpRequest
	{
	public:
		inline static DnsConduit dns_conduit = DNS_CONDUIT_SMART;

		enum Method : uint8_t
		{
			GET,
			POST,
		};

		[[nodiscard]] static std::string method_str(Method method);

		std::function<void(soup::HttpResponse&&)> response_callback = &nullsub<soup::HttpResponse&&>;
		std::function<void(std::string&&)> fail_callback = &nullsub<std::string&&>;
		bool prefer_ipv6 = false;

		HttpRequestBuilder(Method method, std::string&& host, std::string&& path);

		HttpRequestBuilder(const soup::Uri& uri)
			: soup::HttpRequest(uri)
		{
		}

		[[nodiscard]] static std::pair<std::string, std::string> parseHostPathString(const std::string& host_and_path);
		[[nodiscard]] static HttpRequestBuilder fromHostPathString(Method method, const std::string& host_and_path);

		void setPathWithRockstarLocalisation(std::string&& path) noexcept;

		HttpRequestBuilder& addHeader(const char* key, std::string&& value) noexcept
		{
			header_fields.emplace(key, std::move(value));
			return *this;
		}

		HttpRequestBuilder& setResponseCallback(std::function<void(soup::HttpResponse&&)>&& f) noexcept
		{
			response_callback = std::move(f);
			return *this;
		}

		HttpRequestBuilder& setFailCallback(std::function<void(std::string&&)>&& f) noexcept
		{
			fail_callback = std::move(f);
			return *this;
		}

		void dispatch() noexcept;

		static void updateNetConfig(soup::netConfig& conf) SOUP_EXCAL;
	};
#pragma pack(pop)
}
