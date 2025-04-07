#pragma once

#include "CommandInputRid.hpp"

namespace Stand
{
	class CommandGamerRid : public CommandInputRid
	{
	private:
		CommandGamer* const gamer;

	public:
		explicit CommandGamerRid(CommandList* parent, const std::string& menu_name, const std::vector<CommandName>& command_names, CommandGamer* const gamer)
			: CommandInputRid(parent, LIT(std::move(std::string(menu_name).append(" - RID"))), combineCommandNames(command_names, CMDNAME("rid")), NOLABEL, "0"), gamer(gamer)
		{
		}

		void onCommand(Click& click, int64_t rid) final
		{
			gamer->onRidUpdate();
		}
	};
}
