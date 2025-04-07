#include "CommandRemovedPlayers.hpp"
#include "CommandUndoRemoved.hpp"

#include "Hooking.hpp"

namespace Stand
{
	CommandRemovedPlayers::CommandRemovedPlayers(CommandList* const parent)
		: CommandList(parent, LOC("BJ_REMOVED"), {}, LOC("BJ_REMOVED_H"))
	{
	}

	void CommandRemovedPlayers::onActiveListUpdate()
	{
		if (isThisOrSublistCurrentMenuList())
		{
			if (!was_active)
			{
				was_active = true;
				resetChildren();
				{
					for (const auto& p : g_hooking.removed_players)
					{
						this->createChild<CommandUndoRemoved>(p.first);
					}
				}
				processChildrenUpdate();
			}
		}
		else
		{
			was_active = false;
		}
	}
}
