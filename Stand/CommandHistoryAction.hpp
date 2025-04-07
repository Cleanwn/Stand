#pragma once

#include "CommandAction.hpp"

#include "gta_fwddecl.hpp"

namespace Stand
{
	class CommandHistoryAction : public CommandAction
	{
	public:
		explicit CommandHistoryAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL);

		[[nodiscard]] const CommandHistoricPlayer* getHpCmd() const noexcept;
		[[nodiscard]] HistoricPlayer* getHp() const noexcept;
	};
}
