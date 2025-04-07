#include "CommandShowTransitionState.hpp"

#include <fmt/xchar.h>

#include "get_current_time_millis.hpp"
#include "is_session.hpp"
#include "pointers.hpp"
#include "regular_event.hpp"
#include "Renderer.hpp"
#include "ScriptGlobal.hpp"
#include "TransitionHelper.hpp"
#include "TransitionState.hpp"

namespace Stand
{
	static TransitionState last_transition_state = TRANSITION_STATE_EMPTY;
	static time_t show_last_transition_state_until = 0ll;

	CommandShowTransitionState::CommandShowTransitionState(CommandList* const parent)
		: CommandToggle(parent, LOC("SHWTRNSTT"), { CMDNAME("showtransitionstate") })
	{
	}

	void CommandShowTransitionState::onEnable(Click& click)
	{
		if (!is_session_transition_active(true))
		{
			show_last_transition_state_until = (click.isAuto() ? 1ll : (get_current_time_millis() + 3000ll));
		}
		rePresentEvent::registerHandler(&onPresent);
	}

	void CommandShowTransitionState::onDisable(Click& click)
	{
		rePresentEvent::unregisterHandler(&onPresent);
	}

	void CommandShowTransitionState::onPresent()
	{
		if (*pointers::CLoadingScreens_ms_Context != 0)
		{
			return;
		}
		const TransitionState transition_state = ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>();
		if (transition_state == TRANSITION_STATE_EMPTY)
		{
			return;
		}
		bool show_timing = true;
		if (!is_session_transition_active(transition_state))
		{
			SOUP_IF_UNLIKELY (show_last_transition_state_until == 0ll)
			{
				show_last_transition_state_until = (get_current_time_millis() + 3000ll);
			}
			else if (IS_DEADLINE_REACHED(show_last_transition_state_until))
			{
				return;
			}
			show_timing = false;
		}
		else
		{
			show_last_transition_state_until = 0ll;
		}
		if (show_timing)
		{
			const auto time_wasted = TransitionHelper::getTimeWastedOnTransitionState();
			g_renderer.drawCentredTextThisFrame(fmt::format(L"{} ({:0>3}.{}s)", transitionStateToString(transition_state), time_wasted / 1000, (time_wasted / 100) % 10));
		}
		else
		{
			const auto time_remaing = show_last_transition_state_until - get_current_time_millis();
			g_renderer.drawCentredTextThisFrame(fmt::format(L"{} (-{}.{}s)", transitionStateToString(transition_state), time_remaing / 1000, (time_remaing / 100) % 10));
		}
	}

	const wchar_t* CommandShowTransitionState::transitionStateToString(const int transition_state)
	{
		switch (transition_state)
		{
			/*case 0:
				return L"EMPTY";*/

		case 1:
			return L"SP_SWOOP_UP";

		case 2:
			return L"MP_SWOOP_UP";

		case 3:
			return L"CREATOR_SWOOP_UP";

		case 4:
			return L"PRE_HUD_CHECKS";

		case 5:
			return L"WAIT_HUD_EXIT";

		case 7:
			return L"SP_SWOOP_DOWN";

		case 8:
			return L"MP_SWOOP_DOWN";

		case 6:
			return L"WAIT_FOR_SUMMON";

		case 9:
			return L"CANCEL_JOINING";

		case 15:
			return L"WAIT_ON_INVITE";

		case 10:
			return L"RETRY_LOADING";

		case 11:
			return L"RETRY_LOADING_SLOT_1";

		case 12:
			return L"RETRY_LOADING_SLOT_2";

		case 13:
			return L"RETRY_LOADING_SLOT_3";

		case 14:
			return L"RETRY_LOADING_SLOT_4";

		case 16:
			return L"PREJOINING_FM_SESSION_CHECKS";

		case 17:
			return L"LOOK_FOR_FRESH_JOIN_FM";

		case 18:
			return L"LOOK_TO_JOIN_ANOTHER_SESSION_FM";

		case 19:
			return L"CONFIRM_FM_SESSION_JOINING";

		case 21:
			return L"CREATION_ENTER_SESSION";

		case 22:
			return L"PRE_FM_LAUNCH_SCRIPT";

		case 23:
			return L"FM_TEAMFULL_CHECK";

		case 24:
			return L"START_FM_LAUNCH_SCRIPT";

		case 25:
			return L"FM_TRANSITION_CREATE_PLAYER";

		case 26:
			return L"IS_FM_AND_TRANSITION_READY";

		case 27:
			return L"FM_SWOOP_DOWN";

		case 29:
			return L"FM_FINAL_SETUP_PLAYER";

		case 28:
			return L"POST_BINK_VIDEO_WARP";

		case 30:
			return L"MOVE_FM_TO_RUNNING_STATE";

		case 31:
			return L"FM_HOW_TO_TERMINATE";

		case 20:
			return L"WAIT_JOIN_FM_SESSION";

		case 32:
			return L"START_CREATOR_PRE_LAUNCH_SCRIPT_CHECK";

		case 33:
			return L"START_CREATOR_LAUNCH_SCRIPT";

		case 34:
			return L"CREATOR_TRANSITION_CREATE_PLAYER";

		case 35:
			return L"IS_CREATOR_AND_TRANSITION_READY";

		case 36:
			return L"CREATOR_SWOOP_DOWN";

		case 37:
			return L"CREATOR_FINAL_SETUP_PLAYER";

		case 38:
			return L"MOVE_CREATOR_TO_RUNNING_STATE";

		case 39:
			return L"PREJOINING_TESTBED_SESSION_CHECKS";

		case 40:
			return L"LOOK_FOR_FRESH_JOIN_TESTBED";

		case 41:
			return L"LOOK_FOR_FRESH_HOST_TESTBED";

		case 42:
			return L"LOOK_TO_JOIN_ANOTHER_SESSION_TESTBED";

		case 43:
			return L"LOOK_TO_HOST_SESSION_TESTBED";

		case 44:
			return L"CONFIRM_TESTBED_SESSION_JOINING";

		case 46:
			return L"START_TESTBED_LAUNCH_SCRIPT";

		case 47:
			return L"TESTBED_TRANSITION_CREATE_PLAYER";

		case 48:
			return L"IS_TESTBED_AND_TRANSITION_READY";

		case 49:
			return L"TESTBED_SWOOP_DOWN";

		case 50:
			return L"TESTBED_FINAL_SETUP_PLAYER";

		case 51:
			return L"MOVE_TESTBED_TO_RUNNING_STATE";

		case 52:
			return L"TESTBED_HOW_TO_TERMINATE";

		case 45:
			return L"WAIT_JOIN_TESTBED_SESSION";

		case 54:
			return L"WAIT_FOR_TRANSITION_SESSION_TO_SETUP";

		case 53:
			return L"QUIT_CURRENT_SESSION_PROMPT";

		case 55:
			return L"TERMINATE_SP";

		case 56:
			return L"WAIT_TERMINATE_SP";

		case 57:
			return L"KICK_TERMINATE_SESSION";

		case 58:
			return L"TERMINATE_SESSION";

		case 59:
			return L"WAIT_TERMINATE_SESSION";

		case 60:
			return L"TERMINATE_SESSION_AND_HOLD";

		case 61:
			return L"TERMINATE_SESSION_AND_MOVE_INTO_HOLDING_STATE";

		case 62:
			return L"TEAM_SWAPPING_CHECKS";

		case 63:
			return L"RETURN_TO_SINGLEPLAYER";

		case 64:
			return L"WAIT_FOR_SINGLEPLAYER_TO_START";

		case 65:
			return L"WAITING_FOR_EXTERNAL_TERMINATION_CALL";

		case 66:
			return L"TERMINATE_MAINTRANSITION";

		case 67:
			return L"WAIT_FOR_DIRTY_LOAD_CONFIRM";

		case 68:
			return L"DLC_INTRO_BINK";

		default:
			return L"UNKNOWN";
		}
	}
}
