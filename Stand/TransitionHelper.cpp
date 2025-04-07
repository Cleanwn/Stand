#include "TransitionHelper.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "Camgr.hpp"
#include "CNetworkSession.hpp"
#include "CommandBlockBlockJoin.hpp"
#include "CommandJoinDesynced.hpp"
#include "CommandPersonalVehicles.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "is_session.hpp"
#include "atStringHash.hpp"
#include "LeaveReasons.hpp"
#include "natives.hpp"
#include "netPeerId.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "script_thread.hpp"
#include "TranscendentVehicle.hpp"
#include "evtTransitionFinishedEvent.hpp"
#include "TransitionState.hpp"

//#include <fmt/format.h>
//#include "Util.hpp"

#ifdef STAND_DEBUG
#include "FileLogger.hpp"
#endif

namespace Stand
{
	time_t TransitionHelper::getTimeWastedOnTransitionState()
	{
		return get_current_time_millis() - ts_started;
	}

	time_t TransitionHelper::getTimeWhenTransitionEnded()
	{
		if (is_session_transition_active(true))
		{
			return TIME_MAX;
		}
		return ts_started;
	}

	time_t TransitionHelper::getTimeSinceTransitionEnded()
	{
		return GET_MILLIS_SINCE(getTimeWhenTransitionEnded());
	}

	static TransitionState prev_ts = TRANSITION_STATE_EMPTY;

	void TransitionHelper::onTick()
	{
		if (*pointers::CLoadingScreens_ms_Context != 0)
		{
			return;
		}
		auto* const tsptr = ScriptGlobal(GLOBAL_TRANSITION_STATE).as<TransitionState*>();
		const auto ts = *tsptr;
		if (ts != prev_ts)
		{
			if (ts == TRANSITION_STATE_WAIT_JOIN_FM_SESSION || prev_ts == TRANSITION_STATE_WAIT_JOIN_FM_SESSION)
			{
				if (g_hooking.block_bail_rid)
				{
					ScriptGlobal(GLOBAL_GIVE_WEAPONS).set<BOOL>(TRUE);
				}
				if (g_gui.player_list != nullptr)
				{
					g_gui.updatePlayerCount();
				}
				CommandBlockBlockJoin::session_players.clear();
				CommandBlockBlockJoin::pending_object_ids.clear();
			}
			if (ts == TRANSITION_STATE_LOOK_TO_JOIN_ANOTHER_SESSION_FM)
			{
				// Clean up iSceneID in GPBD_Interactions, so e.g. if a player previously was in slot 0 with a scene id,
				// and we are now entering slot 0, we don't have a scene id, as that would cause us to be unable to use chat.
				for (compactplayer_t p = 0; p != MAX_PLAYERS; ++p)
				{
					ScriptGlobal(GPBD_Interactions).at(p, GLOBAL_INTERACT_SIZE).at(GLOBAL_INTERACT_iSceneId).set(-1);
				}
			}
			ts_started = get_current_time_millis();
			prev_ts = ts;
			//Util::toast(fmt::format("Transition State is now {}", (int)ts), TC_SCRIPT_NOYIELD, TOAST_ALL);
			//Util::toast(fmt::format("Transition State is now {} ({})", (int)ts, ScriptGlobal(1574990).get<int>()), TC_SCRIPT_NOYIELD, TOAST_ALL);
			if (ts == TRANSITION_STATE_TERMINATE_MAINTRANSITION)
			{
				auto num_gcd = RemoteGamer::gc.tick();
#ifdef STAND_DEBUG
				g_logger.log(fmt::format("GC evicted {} objects; {} remain.", num_gcd, RemoteGamer::gc.getNumObjects()));
#endif
				evtTransitionFinishedEvent::trigger({});
				if (CommandPersonalVehicles::instance)
				{
					CommandPersonalVehicles::instance->refresh();
				}
			}
		}
		else if (fix_stucks)
		{
			switch (ts)
			{
			case TRANSITION_STATE_WAIT_FOR_SINGLEPLAYER_TO_START:
				if (getTimeWastedOnTransitionState() > 3000)
				{
					STREAMING::STOP_PLAYER_SWITCH();
				}
				break;

			case TRANSITION_STATE_IS_FM_AND_TRANSITION_READY:
				if (getTimeWastedOnTransitionState() > 20000) // Used to be 6 seconds, but if spawn location is an interior, it can take a while.
				{
					*tsptr = TRANSITION_STATE_FM_SWOOP_DOWN;
				}
				break;

			case TRANSITION_STATE_PRE_FM_LAUNCH_SCRIPT:
				if (getTimeWastedOnTransitionState() > 5000)
				{
					auto* thread = GtaThread::fromHash(ATSTRINGHASH("freemode"));
					if (thread != nullptr)
					{
						thread->m_context.m_state = rage::scrThread::ABORTED;
					}
				}
				break;
			}
		}
	}

	bool TransitionHelper::isPrologueDone()
	{
		return MISC::GET_PROFILE_SETTING(903);
	}

	void TransitionHelper::skipPrologue()
	{
		constexpr int32_t scripts[] = {
			ATSTRINGHASH("flow_controller"),
			ATSTRINGHASH("initial"),
			ATSTRINGHASH("main_persistent"),
			ATSTRINGHASH("mission_repeat_controller"),
			ATSTRINGHASH("standard_global_reg"),
		};
		for (const auto& script : scripts)
		{
			auto* thread = GtaThread::fromHash(script);
			if (thread != nullptr)
			{
				thread->spoofAs([]
				{
					STATS::SET_PROFILE_SETTING_PROLOGUE_COMPLETE();
				});
			}
		}
	}

#define FAKE_CAM false

#if FAKE_CAM
	static void release_cam(bool& hijacked_cam)
	{
		if (hijacked_cam)
		{
			Camgr::unlock_pos();
			Camgr::unlock_rot();
			hijacked_cam = false;
		}
	}
#endif

	bool TransitionHelper::doTransition(SessionType session_type, bool end_when_almost_done)
	{
#if FAKE_CAM
		{
			auto pos = g_cam_ped.getPos();
			pos.z = 800.0f;
			Camgr::lock_pos(std::move(pos));
		}
		Camgr::lock_rot({ -90.0f, 0.0f, 0.0f });
#endif

		if (!isPrologueDone())
		{
			skipPrologue();
		}

		TranscendentVehicle::saveFromPlayerState();

		for (const auto& p : AbstractPlayer::listExcept(true))
		{
			LeaveReasons::getEntry(p.getPeerId()).no_removed_gamer_notify = true;
		}

		if (session_type == STAND_QUIT_TO_SP)
		{
			if (*ScriptGlobal(GLOBAL_TRANSITION_STATE).as<TransitionState*>() != TRANSITION_STATE_TERMINATE_MAINTRANSITION)
			{
				beAlone();
				do
				{
					Script::current()->yield();
				} while (*pointers::CLoadingScreens_ms_Context != 0
					|| *ScriptGlobal(GLOBAL_TRANSITION_STATE).as<TransitionState*>() != TRANSITION_STATE_TERMINATE_MAINTRANSITION
					);
				Script::current()->yield(1000);
			}

			*ScriptGlobal(GLOBAL_NEW_SESSION_QUIT_TO_SP).as<int*>() = -1;
			*ScriptGlobal(GLOBAL_NEW_SESSION_UNK1).as<int*>() = -1;
			*ScriptGlobal(GLOBAL_NEW_SESSION_UNK3).as<int*>() = 3;
			*ScriptGlobal(GLOBAL_NEW_SESSION).as<int*>() = 1;
		}
		else if (session_type == STAND_CREATOR_MODE)
		{
			*ScriptGlobal(GLOBAL_NEW_SESSION_UNK1).as<int*>() = 2;
			*ScriptGlobal(GLOBAL_NEW_SESSION_QUIT_TO_SP).as<int*>() = 2;
			*ScriptGlobal(GLOBAL_NEW_SESSION).as<int*>() = 1;
		}
		else
		{
			*ScriptGlobal(GLOBAL_NEW_SESSION_QUIT_TO_SP).as<int*>() = 0;
			*ScriptGlobal(GLOBAL_NEW_SESSION_SCTV).as<BOOL*>() = (session_type == STAND_SCTV || session_type == STAND_NEW_PUBLIC_AS_SPECTATOR);
			*ScriptGlobal(GLOBAL_NEW_SESSION_UNK1).as<int*>() = 0;
			if (session_type == STAND_CHARACTER_SELECTION)
			{
				*ScriptGlobal(GLOBAL_NEW_SESSION_UNK2).as<int*>() = 1;
				*ScriptGlobal(GLOBAL_NEW_SESSION_UNK3).as<int*>() = 4;
				*ScriptGlobal(GLOBAL_NEW_SESSION).as<int*>() = 65;
			}
			else
			{
				*ScriptGlobal(GLOBAL_NEW_SESSION_UNK2).as<int*>() = 0;
				*ScriptGlobal(GLOBAL_NEW_SESSION_UNK3).as<int*>() = -1;
				if (session_type == STAND_SCTV)
				{
					*ScriptGlobal(GLOBAL_NEW_SESSION_TYPE).as<int*>() = 13;
				}
				else if (session_type == STAND_NEW_PUBLIC_AS_SPECTATOR)
				{
					*ScriptGlobal(GLOBAL_NEW_SESSION_TYPE).as<int*>() = NEW_PUBLIC;
				}
				else
				{
					*ScriptGlobal(GLOBAL_NEW_SESSION_TYPE).as<int*>() = session_type;
				}
				*ScriptGlobal(GLOBAL_NEW_SESSION).as<int*>() = 1;
			}
		}
		bool went_to_sp = false;
#if FAKE_CAM
		bool hijacked_cam = true;
#endif
		uint8_t off_ticks = 0;
		while (true)
		{
			Script::current()->yield();
			if (*pointers::CLoadingScreens_ms_Context == 0)
			{
				const auto ts = *ScriptGlobal(GLOBAL_TRANSITION_STATE).as<TransitionState*>();
				if (is_session_transition_active(ts, !end_when_almost_done))
				{
					off_ticks = 0;
					switch (ts)
					{
					case TRANSITION_STATE_RETURN_TO_SINGLEPLAYER:
					case TRANSITION_STATE_WAIT_FOR_SINGLEPLAYER_TO_START:
						went_to_sp = true;
						break;

#if FAKE_CAM
					case TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER:
						release_cam(hijacked_cam);
						break;
#endif
					}
				}
				else
				{
					if (++off_ticks > 40)
					{
						break;
					}
				}
			}
		}
		(*pointers::network_session)->setMatchmakingGroupSlots(session_type);
#if FAKE_CAM
		release_cam(hijacked_cam);
#endif
		return went_to_sp;
	}

	void TransitionHelper::abortTransition()
	{
		if (GtaThread::fromHash(ATSTRINGHASH("freemode")) == nullptr)
		{
			ScriptGlobal(GLOBAL_TRANSITION_STATE).set<int>(TRANSITION_STATE_START_FM_LAUNCH_SCRIPT);
			do
			{
				Script::current()->yield();
			} while (GtaThread::fromHash(ATSTRINGHASH("freemode")) == nullptr);
		}
		ScriptGlobal(GLOBAL_TRANSITION_STATE).set<int>(TRANSITION_STATE_MP_SWOOP_DOWN);
		*ScriptGlobal(GLOBAL_NEW_SESSION).as<int*>() = 0;
	}

	void TransitionHelper::forceQuit()
	{
		*ScriptGlobal(GLOBAL_NEW_SESSION).as<int*>() = 0;
		NETWORK::SHUTDOWN_AND_LOAD_MOST_RECENT_SAVE();
	}

	void TransitionHelper::beAlone(int remove_reason)
	{
		beAlone(AbstractPlayer::listExcept(true), remove_reason);
	}

	void TransitionHelper::beAlone(const std::vector<AbstractPlayer>& other_players, int remove_reason)
	{
		CommandJoinDesynced::amnesty = true;
		for (const auto& p : other_players)
		{
			LeaveReasons::getEntry(p.getPeerId()).no_removed_gamer_notify = true;
			if (!p.isHost())
			{
				p.desyncBealone();
			}
		}
		for (const auto& p : other_players)
		{
			if (p.isHost())
			{
				p.remoteDesync(g_player.getGamerHandle(), remove_reason);
				p.desyncBealone();
			}
		}
	}
}
