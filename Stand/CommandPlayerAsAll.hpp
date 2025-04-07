#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandPlayerAsAll : public CommandAction
	{
	public:
		explicit CommandPlayerAsAll(CommandList* const parent)
			: CommandAction(parent, LOC("CMDBHLFALL"), CMDNAMES_OBF("asall"), NOLABEL, CMDFLAGS_ACTION, COMMANDPERM_NEUTRAL)
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (auto cmd = click.getCommandIssuableFromArg(args))
			{
				click.issuer_is_explicit = true;

				for (auto& plr : PP_PTR->getPlayers(false))
				{
					click.issued_for = plr;
					cmd->onCommand(click, args);
				}
			}

			args.clear();
		}
	};
}
