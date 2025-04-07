#include "GlobalsWatcher.hpp"

#include <soup/bitutil.hpp>

#include "AbstractEntity.hpp"
#include "CNetGamePlayer.hpp"
#include "FlowEvent.hpp"
#include "FlowEventReaction.hpp"
#include "natives.hpp"
#include "ScriptEventTaxonomy.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"
#include "Util.hpp"

namespace Stand
{
	const std::pair<int, int> broadcast_vars[] = {
		// Player broadcast
		{ GPBD_FM_2, 32 },
		{ GPBD_Interactions, 32 },
		{ GPBD_FM, 32 },
		{ GSBD_FM_IMPORTANT_ARRAY, 95 },
		{ GPBD_MissionName, 32 },
		{ GPBD_Kicking, 32 },
		{ GPBD_SCTV, 32 },
		{ GPBD_FM_HeistPlanning, 32 },
		{ g_AMC_playerBD, 32 },
		{ GPBD_FM_3, 32 },
		{ GlobalplayerBD, 32 },
		{ GPBD_FM_4, 32 },
		// Host broadcast
		{ GSBD_BlockC, 32 },
		{ GLOBAL_BLOCK_C_IMPORTANT_ARRAY, 26 },
		{ GlobalServerBD_BlockC_g_iAssignedCarMeetParkingSpots, 32 },
		{ GSBD_MissionRequest, 16 },
		{ GSBD_MissionList, 16 },
		{ GSBD_ExclusionAreas, 16 },
		{ GSBD_Betting, 32 },
		{ GSBD_HoldUp, 20 },
		{ GSBD_HoldUp + 41, 20 },
		{ GSBD_SyncedInteractions, 32 },
		{ GSBD_Kicking, 32 },
		{ GSBD_PropertyInstances, 33 },
		{ GPBD_CasinoHeist, 32 },
		{ GSBD_RandomEvents + 1, 20 },
		{ GPBD_HeistIsland, 32 },
		{ GPBD_NetHeistPlanningGeneric, 32 },
		{ GPBD_Flow, 32 },
	};

	void GlobalsWatcher::onTickSessionActive()
	{
		// Get SH directly to avoid latency with pool's closed kick, where AbstractPlayer::getScriptHost might return previous SH
		const AbstractPlayer sh = NETWORK::NETWORK_GET_HOST_OF_SCRIPT("freemode", -1, 0);

		// Watch for "pool's closed" kick
		if (sh != g_player)
		{
			if (ScriptGlobal(GSBD).get<int>() == 5)
			{
				auto reactions = sh.getReactions(FlowEvent::SE_KICK, true, true);
				if (reactions & REACTION_BLOCK)
				{
					ScriptGlobal(GSBD).set<int>(4);
				}
				sh.applyReactionsIn(reactions, FlowEvent::SE_KICK, Codename("G0").toString());
			}
		}

		// Watch for broadcast variable size poisoning (NETWORK_FINISH_BROADCASTING_DATA)
		for (const auto& bv : broadcast_vars)
		{
			SOUP_IF_UNLIKELY (ScriptGlobal(bv.first).get<int>() != bv.second)
			{
#ifdef STAND_DEBUG
				Util::toast(fmt::format("Global_{} was set to {}, fixing to {}", bv.first, ScriptGlobal(bv.first).get<int>(), bv.second), TOAST_ALL);
#endif
				auto reactions = sh.getReactions(FlowEvent::SE_KICK, true, true);
				if (reactions & REACTION_BLOCK)
				{
					ScriptGlobal(bv.first).set<int>(bv.second);
				}
				sh.applyReactionsIn(reactions, FlowEvent::SE_KICK, Codename("G1").toString());
			}
		}

		// Watch for script host kick via vote kick mechanism
		if (*ScriptGlobal(GLOBAL_VOTE_KICKED).at(g_player, 1).as<int*>() == 1
			&& g_player.canBeVoteKickedNaive()
			&& !g_player.hasEnoughVotesToKick()
			)
		{
			// May be resynched by a new script host so only saying it's an unsual event
			auto reactions = sh.getReactions(FlowEvent::SE_UNUSUAL, true, true);
			//if (reactions & REACTION_BLOCK)
			{
				*ScriptGlobal(GLOBAL_VOTE_KICKED).at(g_player, 1).as<int*>() = 0;
			}
			sh.applyReactionsIn(reactions, FlowEvent::SE_UNUSUAL, Codename("G4").toString());
		}

		// Watch for eMH_MODEL_HIDE_BITSET_CrashSite2_activate_weather
		if (notify_thunder_request
			&& !((*ScriptGlobal(GlobalServerBD_FM_events).at(g_events_sGBWorkData).at(g_events_sGBWorkData_iModelHideBitset).at(0, 1).as<int*>() >> eMH_MODEL_HIDE_BITSET_CrashSite2_activate_weather) & 1)
			)
		{
			for (const auto& p : AbstractPlayer::listAll())
			{
				if (((*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_MHBITSET).at(0, 1).as<int*>() >> eMH_MODEL_HIDE_BITSET_CrashSite2_activate_weather) & 1)
					&& !((did_notify_thunder >> p) & 1)
					)
				{
					did_notify_thunder |= (1 << p);
					Util::toast(LANG_FMT("THDR_N_T", p.getName()));
				}
			}
		}
		else
		{
			for (auto i = did_notify_thunder; i != 0; soup::bitutil::unsetLeastSignificantSetBit(i))
			{
				auto p = soup::bitutil::getLeastSignificantSetBit(i);
				if (!((*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_MHBITSET).at(0, 1).as<int*>() >> eMH_MODEL_HIDE_BITSET_CrashSite2_activate_weather) & 1))
				{
					did_notify_thunder &= ~(1 << p);
				}
			}
		}

		// Watch for SH kick from interior
		if (auto t = GtaThread::fromHash(ATSTRINGHASH("am_mp_property_int")))
		{
			if (*ScriptLocal(t, LOCAL_INT_SHDATA + LOCAL_INT_SHDATA_KICK).as<int*>() & 1)
			{
				AbstractPlayer int_sh = t->getHost();
				if (g_player.getInteriorId() != int_sh.getInteriorId())
				{
					auto reactions = int_sh.getReactions(FlowEvent::SE_INTERIORKICK);
					if (reactions & REACTION_BLOCK)
					{
						*ScriptLocal(t, LOCAL_INT_SHDATA + LOCAL_INT_SHDATA_KICK).as<int*>() &= ~1;
					}
					int_sh.applyReactionsIn(reactions, FlowEvent::SE_INTERIORKICK, Codename("G0").toString());
				}
			}
		}

		// Watch for kick from vehicle
		// After setting the global, the following SE may also be sent for instant effect:
		// - SCRIPT_EVENT_GB_REFRESH_LIMO_LOCKS
		// - SCRIPT_EVENT_GANG_STATUS_CHANGED (f_2 = 0)
		if (auto netId = NETWORK::VEH_TO_NET(g_player_veh); netId != NETWORK_INVALID_OBJECT_ID)
		{
			for (const auto& p : AbstractPlayer::listExcept(true))
			{
				if (ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_bossLimo).get<int>() == netId
					&& g_player.getBoss() != p
					)
				{
					auto reactions = p.getReactions(FlowEvent::SE_PVKICK);
					if (reactions & REACTION_BLOCK)
					{
						ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_bossLimo).set<int>(0);
					}
					p.applyReactionsIn(reactions, FlowEvent::SE_PVKICK, Codename("G0").toString());
				}
			}
		}

		const auto players = AbstractPlayer::listExcept(true);

		// Watch for freemode death kick via globals
		if (g_player == sh)
		{
			for (const auto& p : players)
			{
				// Related to SCRIPT_EVENT_GB_BOSS_WORK_REQUEST_SERVER
				int* pMissionId = ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_MISSION).as<int*>();
				if (*pMissionId != -1)
				{
					// The mission id needs to be within freemode func_8858 (1.68-3095).
					if (!ScriptEventTaxonomy::isValidFreemodeMissionId(*pMissionId))
					{
						auto reactions = p.getReactions(FlowEvent::SE_UNUSUAL, true, true);
						if (reactions & REACTION_BLOCK)
						{
							*pMissionId = -1;
						}
						p.applyReactionsIn(reactions, FlowEvent::SE_UNUSUAL, Codename("G0").toString());
					}
				}

				// A player sets this global when receiving SCRIPT_EVENT_LAUNCH_SYNCED_INTERACTION
				int* pSceneID = ScriptGlobal(GPBD_Interactions).at(p, GLOBAL_INTERACT_SIZE).at(GLOBAL_INTERACT_iSceneId).as<int*>();
				if (*pSceneID >= 123)
				{
					auto reactions = p.getReactions(FlowEvent::SE_UNUSUAL, true, true);
					if (reactions & REACTION_BLOCK)
					{
						*pSceneID = -1;
					}
					p.applyReactionsIn(reactions, FlowEvent::SE_UNUSUAL, Codename("G1").toString());
				}

				// Related to SCRIPT_EVENT_PLAYER_AVAILABLE_FOR_BETTING. Gets copied to ScriptGlobal(GSBD_Betting).at(p, 10).at(9) by the SH, then causes problems.
				int* pUnkBettingIdx = ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_UNKBETTINGIDX).as<int*>();
				if (*pUnkBettingIdx >= MAX_PLAYERS)
				{
					auto reactions = p.getReactions(FlowEvent::SE_UNUSUAL, true, true);
					if (reactions & REACTION_BLOCK)
					{
						*pUnkBettingIdx = 0;
					}
					p.applyReactionsIn(reactions, FlowEvent::SE_UNUSUAL, Codename("G2").toString());
				}
			}
		}

		// Detect indirect money drops & kicks.
		for (const auto& p : players)
		{
			for (int i = 0; i != 10; ++i)
			{
				if (ScriptGlobal(GPBD_FM_4).at(p, GPBD_FM_4_SIZE).at(GPBD_FM_4_PICKUP_REQUESTS).at(i, GPBD_FM_4_PICKUP_REQUESTS_SIZE).at(0).get<int>() != 123)
				{
					processIndirectPickup(
						p,
						ScriptGlobal(GPBD_FM_4).at(p, GPBD_FM_4_SIZE).at(GPBD_FM_4_PICKUP_REQUESTS).at(i, GPBD_FM_4_PICKUP_REQUESTS_SIZE).at(0).get<hash_t>(),
						ScriptGlobal(GPBD_FM_4).at(p, GPBD_FM_4_SIZE).at(GPBD_FM_4_PICKUP_REQUESTS).at(i, GPBD_FM_4_PICKUP_REQUESTS_SIZE).at(9).get<int>(),
						ScriptGlobal(GPBD_FM_4).at(p, GPBD_FM_4_SIZE).at(GPBD_FM_4_PICKUP_REQUESTS).at(i, GPBD_FM_4_PICKUP_REQUESTS_SIZE).at(10).get<hash_t>()
					);
				}
			}
		}
#if false // Could also block them here, but defering that to CREATE_NON_NETWORKED_AMBIENT_PICKUP hook
		for (int i = 0; i != 10; ++i)
		{
			SOUP_IF_UNLIKELY (ScriptGlobal(GSBD_BlockC + 2452).at(i, 16).get<int>() != 123
				&& ScriptGlobal(GSBD_BlockC + 2452).at(i, 16).at(10).get<int>() == ATSTRINGHASH("p_poly_bag_01_s")
				)
			{
				ScriptGlobal(GSBD_BlockC + 2452).at(i, 16).set<int>(123);
			}
		}
#endif
	}

	void GlobalsWatcher::processIndirectPickup(const AbstractPlayer& source, hash_t type, int value, hash_t model)
	{
		if (!Util::is_legit_ambient_pickup(type, value, model))
		{
			// Detect kick pickups (modded event instead of kick event because we might not be the target.)
			switch (model)
			{
			case ATSTRINGHASH("bkr_prop_coke_boxeddoll"):
			case ATSTRINGHASH("vw_prop_vw_colle_sasquatch"):
			case ATSTRINGHASH("vw_prop_vw_colle_beast"):
			case ATSTRINGHASH("vw_prop_vw_colle_rsrgeneric"):
			case ATSTRINGHASH("vw_prop_vw_colle_rsrcomm"):
			case ATSTRINGHASH("vw_prop_vw_colle_pogo"):
			case ATSTRINGHASH("vw_prop_vw_colle_prbubble"):
			case ATSTRINGHASH("vw_prop_vw_colle_imporage"):
			case ATSTRINGHASH("vw_prop_vw_colle_alien"):
				if (value > 99)
				{
					source.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("G2").toString());
					return;
				}
				break;

			case ATSTRINGHASH("vw_prop_vw_lux_card_01a"):
				if (value > 53)
				{
					source.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("G2").toString());
					return;
				}
				break;
			}

			std::string extra_data = Codename("G0").toString();
#ifdef STAND_DEV
			extra_data.push_back(':');
			extra_data.append(Util::to_padded_hex_string(type));
			extra_data.push_back(':');
			extra_data.append(fmt::to_string(value));
			extra_data.push_back(':');
			extra_data.append(Util::to_padded_hex_string(model));
#endif
			source.getAndApplyReactionsIn(FlowEvent::SE_INVALID, std::move(extra_data));
		}
	}
}
