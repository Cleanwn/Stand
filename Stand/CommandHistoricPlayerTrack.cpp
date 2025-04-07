#include "CommandHistoricPlayerTrack.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"
#include "RemoteGamer.hpp"

namespace Stand
{
	CommandHistoricPlayerTrack::CommandHistoricPlayerTrack(CommandList* parent)
		: CommandListSelect(parent, LOC("TRK"), {}, LOC("TRK_H"), {
			{ 0, LOC("DOFF") },
			{ 1, LOC("ON") },
			{ 2, LOC("TRK_FF"), {}, LOC("TRK_FF_H") },
		}, parent->as<CommandHistoricPlayer>()->player->getTracking().value, CMDFLAGS_TOGGLE | CMDFLAG_TEMPORARY)
	{
	}

	void CommandHistoricPlayerTrack::onChange(Click& click, long long prev_value)
	{
		if (value == 0 || click.isUltimateEdition())
		{
			if (parent->as<CommandHistoricPlayer>()->player->setTracking(HistoricPlayer::Tracking{ (uint8_t)value }))
			{
				PlayerHistory::save();
				return;
			}
			click.setResponse(LOC("GENFAIL"));
		}
		onChangeImplUnavailable(click, prev_value);
	}

	void CommandHistoricPlayerTrack::onTickInGameViewport()
	{
		if (isFocused())
		{
			// If the game is running at a really high framerate, this can cause this->value to be overwritten before the onChange handler had a chance to read it, causing the change to fail.
		}
		else
		{
			setValueIndication(parent->as<CommandHistoricPlayer>()->player->getTracking().value);
		}
	}

	void CommandHistoricPlayerTrack::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
