#include "CommandPlayerBail.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "CommandSlider.hpp"
#include "Gui.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	CommandPlayerBail::CommandPlayerBail(CommandList* const parent)
		: CommandPlayerToggle(parent, LOC("PLYBAIL"), { CMDNAME("bail") }, NOLABEL, COMMANDPERM_FRIENDLY)
	{
	}

	void CommandPlayerBail::onEnable(Click& click)
	{
		DEF_P2;
		if (pp->single && pp->getPlayers(true).at(0) == g_player)
		{
			auto click_ = click.deriveAuto();
			this->setStateBool(click_, false);
			auto* const self = ((CommandList*)g_gui.root_list->resolveChildByMenuName(LOC("SLF")));
			((CommandSlider*)self->recursivelyResolveChildByMenuName(LOC("WNTD")))->setValue(click_, 0);
			auto* const lock_wanted = ((CommandToggleNoCorrelation*)self->recursivelyResolveChildByMenuName(LOC("LKWNTD")));
			lock_wanted->setStateBool(click_, true);
			if (click.isMenu())
			{
				lock_wanted->focus(click.thread_context);
			}
		}
		else
		{
			pp->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [](const std::vector<AbstractPlayer>& players)
			{
				for (const AbstractPlayer p : players)
				{
					if (p.getWantedLevel() > 0)
					{
						p.setWantedLevel(0);
					}
				}
			});
		}
	}
}
