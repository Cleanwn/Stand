#include "CommandHistoryCopy.hpp"

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"
#include "RemoteGamer.hpp"
#include "SessionSpoofing.hpp"

namespace Stand
{
	CommandHistoryCopy::CommandHistoryCopy(CommandList* parent)
		: CommandReadonlyValue(parent, LOC("CPYSESS_F"))
	{
	}

	void CommandHistoryCopy::onClick(Click& click)
	{
		if (SessionSpoofing::copy_from != nullptr)
		{
			PlayerHistory::findCommand(PlayerHistory::findRID(SessionSpoofing::copy_from->rid))->redirectOpen(click, this);
		}
	}

	void CommandHistoryCopy::onTickInGameViewport()
	{
		if (SessionSpoofing::copy_from != nullptr)
		{
			setValue(PlayerHistory::findRID(SessionSpoofing::copy_from->rid)->name);
		}
		else
		{
			setValue(LANG_GET_W("NA"));
		}
	}

	void CommandHistoryCopy::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
