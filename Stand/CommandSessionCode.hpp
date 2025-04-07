#pragma once

#include "CommandList.hpp"

#include <optional>

namespace Stand
{
	class CommandSessionCode : public CommandList
	{
	public:
		explicit CommandSessionCode(CommandList* parent);

		[[nodiscard]] static std::string getGsinfo();
		[[nodiscard]] static bool hasSessionCode();
		[[nodiscard]] static bool hasSessionCode(const std::string& gsinfo);
		[[nodiscard]] static std::optional<std::string> getSessionCode();
		[[nodiscard]] static std::optional<std::string> getSessionCodeOrStartRequest(const std::string& gsinfo);
		[[nodiscard]] static std::optional<std::string> getSessionCodeOrStartRequest(bool& applicable);
		[[nodiscard]] static std::string getSessionCodeForUser();
		[[nodiscard]] static std::string getSessionCodeForUser(int64_t gstype, const std::string& gsinfo);
		[[nodiscard]] static bool isSessionTypeViable(int64_t gstype) noexcept;
	};
}
