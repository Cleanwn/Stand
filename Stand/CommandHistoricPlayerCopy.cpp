#include "CommandHistoricPlayerCopy.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"
#include "RemoteGamer.hpp"
#include "SessionSpoofing.hpp"
#include "Tunables.hpp"

namespace Stand
{
	CommandHistoricPlayerCopy::CommandHistoricPlayerCopy(CommandList* parent, const CommandName& simple_name)
		: CommandToggle(parent, LOC("CPYSESS"), std::vector<CommandName>(1, std::move(CommandName(CMDNAME("redirect")).append(simple_name))), LOC("CPYSESS_H"), false, CMDFLAGS_TOGGLE | CMDFLAG_TEMPORARY)
	{
	}

	void CommandHistoricPlayerCopy::onEnable(Click& click)
	{
		if (click.isUltimateEdition())
		{
			if (!g_tunables.getBool(TUNAHASH("DISABLE_SESSION_SPOOFING")))
			{
				auto* g = RemoteGamer::get(parent->as<CommandHistoricPlayer>()->player->rid, true, RGU_NONE);
				if (g != nullptr)
				{
					if (SessionSpoofing::copy_from != nullptr)
					{
						PlayerHistory::findRID(SessionSpoofing::copy_from->rid)->flags &= ~HP_COPY;
					}
					parent->as<CommandHistoricPlayer>()->player->flags |= HP_COPY;
					PlayerHistory::save();
					SessionSpoofing::copy_from = g;
					return;
				}
				else
				{
					click.setResponse(LOC("GENFAIL"));
				}
			}
			else
			{
				click.setResponse(LOC("CMDDISA"));
			}
		}
		onChangeImplUnavailable(click);
	}

	void CommandHistoricPlayerCopy::onDisable(Click& click)
	{
		parent->as<CommandHistoricPlayer>()->player->flags &= ~HP_COPY;
		PlayerHistory::save();
		SessionSpoofing::copy_from = nullptr;
		SessionSpoofing::onDisable();
	}

	void CommandHistoricPlayerCopy::onTickInGameViewport()
	{
		setOnIndication(parent->as<CommandHistoricPlayer>()->player->flags & HP_COPY);
	}

	void CommandHistoricPlayerCopy::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
