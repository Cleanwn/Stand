#pragma once

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include <string>
#include <vector>

namespace Stand
{
	struct ToastConfig
	{
		bool notify = false;
		bool log_file = false;
		bool log_console = false;
		bool chat = false;
		bool team_chat = false;

		void fromToastFlags(toast_t tf) noexcept;

		[[nodiscard]] toast_t getToastFlags() const noexcept;

		void populateList(CommandList* list, const std::vector<CommandName>& command_names_prefix = {});
	};
}
