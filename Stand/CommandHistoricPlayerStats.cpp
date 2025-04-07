#include "CommandHistoricPlayerStats.hpp"

#include "CommandDivider.hpp"
#include "CommandHistoricPlayer.hpp"
#include "CommandReadonlyValueCopy.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "RemoteStats.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandHistoricPlayerStats::CommandHistoricPlayerStats(CommandList* const parent)
		: CommandListWithOnTickAsActiveList(parent, LOC("STS"), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_TEMPORARY)
	{
		createChild<CommandDivider>(LOC("GENWAIT"));
	}

	void CommandHistoricPlayerStats::onClick(Click& click)
	{
		if (!click.isRegularEdition())
		{
			return click.setResponse(LOC("CMDNEDR"));
		}
		s = RemoteStats::get(parent->as<CommandHistoricPlayer>()->player->rid);
		CommandListWithOnTickAsActiveList::onClick(click);
	}

	void CommandHistoricPlayerStats::onActiveListUpdate()
	{
		CommandListWithOnTickAsActiveList::onActiveListUpdate();
		
		if (!isCurrentMenuListInTab()
			&& !isCurrentWebList()
			&& s
			&& RemoteStats::tryFree(s)
			)
		{
			resetChildren();
			EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
			createChild<CommandDivider>(LOC("GENWAIT"));
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
		}
	}

	void CommandHistoricPlayerStats::onTickAsActiveList()
	{
		if (children.size() == 1
			&& s
			&& !s->isPending()
			)
		{
			resetChildren();
			EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
			if (s->hasData())
			{
				createChild<CommandReadonlyValueCopy>(LOC("MNY_B"), StringUtils::toStringWithThousandsSeparatorW(s->getBankBalance()));
				createChild<CommandReadonlyValueCopy>(LOC("KLLS"), fmt::to_wstring(s->getKills()));
				createChild<CommandReadonlyValueCopy>(LOC("DTHS"), fmt::to_wstring(s->getDeaths()));
				createChild<CommandReadonlyValueCopy>(LOC("KD"), fmt::to_wstring(s->getKillDeathRatio()));
				if (s->getMp0IsActive())
				{
					createChild<CommandDivider>(s->getLastChar() == 0 ? LOC("CHAR1_ACT") : LOC("CHAR1"));
					createChild<CommandReadonlyValueCopy>(LOC("RNK"), fmt::to_wstring(s->getMp0Rank()));
					createChild<CommandReadonlyValueCopy>(LOC("MNY_W"), StringUtils::toStringWithThousandsSeparatorW(s->getMp0WalletBalance()));
				}
				if (s->getMp1IsActive())
				{
					createChild<CommandDivider>(s->getLastChar() == 1 ? LOC("CHAR2_ACT") : LOC("CHAR2"));
					createChild<CommandReadonlyValueCopy>(LOC("RNK"), fmt::to_wstring(s->getMp1Rank()));
					createChild<CommandReadonlyValueCopy>(LOC("MNY_W"), StringUtils::toStringWithThousandsSeparatorW(s->getMp1WalletBalance()));
				}
			}
			else
			{
				// A player who has never played before, e.g. test123
				createChild<CommandDivider>(LOC("NDATA"));
			}
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
			processChildrenUpdate();
		}
	}
}
