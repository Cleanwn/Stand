#include "ScriptHostUtil.hpp"

#include "AbstractPlayer.hpp"
#include "atStringHash.hpp"
#include "get_current_time_millis.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "script_thread.hpp"
#include "TransitionHelper.hpp"
#include "TransitionState.hpp"

namespace Stand
{
	bool ScriptHostUtil::canSafelyTakeIt()
	{
		return (
			ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() == TRANSITION_STATE_TERMINATE_MAINTRANSITION
			|| ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() == TRANSITION_STATE_MP_SWOOP_DOWN
			|| (ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() == TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER
				&& TransitionHelper::getTimeWastedOnTransitionState() > 5'000
				&& !AbstractPlayer::getScriptHost().exists()
				)
			);
	}

	void ScriptHostUtil::takeIt()
	{
		if (g_player.isScriptHost())
		{
			return;
		}
		takeItImpl();
	}

	bool ScriptHostUtil::takeItYielding()
	{
		if (g_player.isScriptHost())
		{
			return true;
		}
		if (takeItImpl())
		{
			do
			{
				Script::current()->yield();
				if (g_player.isScriptHost())
				{
					return true;
				}
			} while (GET_MILLIS_SINCE(last_script_host_request) < 6000);
		}
		return false;
	}

	bool ScriptHostUtil::takeItImpl()
	{
		if (GET_MILLIS_SINCE(last_script_host_request) < 10000
			|| !GtaThread::fromHash(ATSTRINGHASH("freemode"))->snatchScriptHost()
			)
		{
			return false;
		}
		last_script_host_request = get_current_time_millis();
		return true;
	}

	AbstractPlayer ScriptHostUtil::getViableSh()
	{
		for (const auto& p : AbstractPlayer::listExcept(true))
		{
			if (GET_MILLIS_SINCE(pool_closed_at[p]) > 30000)
			{
				return p;
			}
		}
		return g_player;
	}
}
