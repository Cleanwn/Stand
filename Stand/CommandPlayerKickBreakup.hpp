#pragma once

#include "CommandPlayerActionAggressiveCustom.hpp"

#define ENABLE_BAN_KICK false

namespace Stand
{
	class CommandPlayerKickBreakup : public CommandPlayerActionAggressiveCustom
	{
	private:
		const int remove_reason;

	public:
		explicit CommandPlayerKickBreakup(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int remove_reason);

		void onClick(Click& click) final;
	};
}
