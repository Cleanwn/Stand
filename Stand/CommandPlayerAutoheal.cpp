#include "CommandPlayerAutoheal.hpp"

#include "AbstractPlayer.hpp"
#include "Gui.hpp"

namespace Stand
{
	CommandPlayerAutoheal::CommandPlayerAutoheal(CommandList* const parent)
		: CommandPlayerToggle(parent, LOC("DEMIGOD"), { CMDNAME("autoheal") }, LOC("DEMIGOD_H_O"), COMMANDPERM_FRIENDLY)
	{
	}

	void CommandPlayerAutoheal::onEnable(Click& click)
	{
		DEF_P2;
		if (pp->single && pp->getPlayers(true).at(0) == g_player)
		{
			auto click_ = click.deriveAuto();
			this->setStateBool(click_, false);
			auto* const demigod = ((CommandToggleNoCorrelation*)((CommandList*)g_gui.root_list->resolveChildByMenuName(LOC("SLF")))->recursivelyResolveChildByMenuName(LOC("DEMIGOD")));
			demigod->setStateBool(click_, true);
			if (click.isMenu())
			{
				demigod->focus(click.thread_context);
			}
		}
		else
		{
			pp->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [](const std::vector<AbstractPlayer>& players)
			{
				for (const auto& p : players)
				{
					p.tickAutoHeal();
				}
			});
		}
	}
}
