#include "CommandPlayerAmmo.hpp"

#include <fmt/core.h>

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "CommandToggle.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "PlayerProvider.hpp"
#include "pointers.hpp"
#include "weapons.hpp"

namespace Stand
{
	CommandPlayerAmmo::CommandPlayerAmmo(CommandList* const parent)
		: CommandOnPlayer<CommandActionScript>(parent, LOC("PLYAMMO"), { CMDNAME("ammo") }, LOC("PLYAMMO_H"), 0, COMMANDPERM_FRIENDLY)
	{
		PC_PTR->registerCommand(this);
	}

	void CommandPlayerAmmo::onClickScriptThread(Click& click)
	{
		DEF_P2;
		if (pp->single && pp->getPlayers(true).at(0) == g_player)
		{
			auto* const demigod = ((CommandToggleNoCorrelation*)((CommandList*)g_gui.root_list->resolveChildByMenuName(LOC("SLF")))->recursivelyResolveChildByMenuName(LOC("INFAMMO")));
			auto click_ = click.deriveAuto();
			demigod->setStateBool(click_, true);
			if (click.isMenu())
			{
				demigod->focus(click.thread_context);
			}
		}
		else
		{
			for (const AbstractPlayer p : pp->getPlayers(false))
			{
				if (p.hasPed())
				{
					auto weapon = Weapon::find(WEAPON::GET_SELECTED_PED_WEAPON(p.getPed()));
					if (weapon != nullptr && weapon->ammo_reward_hash != 0)
					{
						p.givePickupReward(weapon->ammo_reward_hash);
					}
					else
					{
						if (pp->single)
						{
							click.setResponse(LIT(LANG_FMT("PLYAMMO_T", p.getName())));
						}
					}
				}
			}
		}
	}
}
