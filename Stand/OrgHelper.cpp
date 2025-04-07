#include "OrgHelper.hpp"

#include "AbstractPlayer.hpp"
#include "script_events.hpp"
#include "ScriptGlobal.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	void OrgHelper::join(player_t boss)
	{
		join_state = PENDING;
		ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_NEXT_BOSS).set<>(boss);

		uint64_t args[] = {
			Util::i32_to_u64_trickery(SE_CEO_JOINREQ),
			g_player,
		};
		TSE(1 << boss, args);
	}

	bool OrgHelper::hasSentJoinRequest() noexcept
	{
		return join_state == PENDING;
	}

	player_t OrgHelper::getBoss() noexcept
	{
		return ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_NEXT_BOSS).get<player_t>();
	}

	bool OrgHelper::hasBossRespondedToJoinRequest() noexcept
	{
		return join_state == ACCEPTED
			|| join_state == REJECTED
			;
	}

	bool OrgHelper::hasBossAcceptedJoinRequest() noexcept
	{
		const bool accepted = (join_state == ACCEPTED);
		join_state = IDLE; // Clean up now since we won't get another call for this flow
		return accepted;
	}

	void OrgHelper::processJoinResponse(bool accepted) noexcept
	{
		join_state = (accepted ? ACCEPTED : REJECTED);
	}

	void OrgHelper::leave() noexcept
	{
		ScriptGlobal(GLOBAL_LEAVE_ORG).set<int>(g_player.isInMc() ? 6 : 5); // 5 = ceo, 6 = mc
	}
}
