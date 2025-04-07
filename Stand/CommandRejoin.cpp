#include "CommandRejoin.hpp"

#include "AbstractPlayer.hpp"
#include "CommandPlayer.hpp"
#include "JoinUtil.hpp"
#include "RemoteGamer.hpp"
#include "SessionSpoofing.hpp"
#include "TransitionHelper.hpp"

namespace Stand
{
	CommandRejoin::CommandRejoin(CommandList* parent)
		: CommandAction(parent, LOC("RJNSESS"), CMDNAMES("rejoin"))
	{
	}

	void CommandRejoin::onClick(Click& click)
	{
		ensureYieldableScriptThread(click, []
		{
			if (!g_player.isHost()
				&& !AbstractPlayer::listExcept(true).empty()
				)
			{
				JoinUtil::join(SessionSpoofing::getRealSessionInfo(), JM_NUTS_REJOIN);
				return;
			}
			RemoteGamer* g = RemoteGamer::get(CommandPlayer::trampoline_rid, true, RGU_BULK);
			if (g == nullptr)
			{
				return;
			}
			TransitionHelper::doTransition(INVITE_ONLY);

			// Give the user the option to back out by not using nuts method. We also don't need it because we just transitioned.
			g->useAsTrampoline(false);
		});
	}
}
