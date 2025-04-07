#pragma once

namespace Stand
{
	struct BgScript
	{
		static void boot(const std::string& code);
		static void bootFromBase64(const std::string& data);
		static void bootFromBuiltin();
		static void shutdown();

		[[nodiscard]] static bool hasFunction(const char* func) noexcept;
		static void invokeOptionalHook(const char* func) noexcept;
		[[nodiscard]] static bool query(const char* func, int64_t arg) noexcept;
		[[nodiscard]] static bool query(const char* func, const std::string& arg) noexcept;
		[[nodiscard]] static bool query(const char* func, int64_t a1, const std::string& a2, const std::string& a3) noexcept;
	};
}
