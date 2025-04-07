#pragma once

#include "CommandAction.hpp"
#include "CommandOnPlayer.hpp"

namespace Stand
{
	class CommandPlayerAction : public CommandOnPlayer<CommandAction>
	{
	protected:
		explicit CommandPlayerAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, CommandPerm perm = COMMANDPERM_USERONLY, commandflags_t flags = CMDFLAGS_ACTION);

		template <typename T>
		[[nodiscard]] static bool setWhitelistTargeting(Whitelist<T>& whitelist, const std::vector<AbstractPlayer>& targets)
		{
			// Ensure whitelist is not currently in use
			if (whitelist.isActive())
			{
				return false;
			}

			// Add suitable targets to the whitelist
			for (const auto& p : targets)
			{
				if (!p.isRockstarAdmin())
				{
					whitelist.add(p.get<T>());
				}
			}

			// Proceed if the whitelist contains at least 1 player now
			return whitelist.isActive();
		}
	};
}
