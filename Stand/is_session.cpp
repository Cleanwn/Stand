#include "is_session.hpp"

#include "CNetworkSession.hpp"
#include "pointers.hpp"
#include "ScriptGlobal.hpp"
#include "TransitionState.hpp"

namespace Stand
{
    bool is_session_started() noexcept
    {
        return *pointers::is_session_started;
    }

	bool is_session_started_and_transition_finished()
	{
		return is_session_started() && !is_session_transition_active(true);
	}

	bool is_session_transition_active(bool when_in_doubt, bool when_almost_done) noexcept
	{
		if (ScriptGlobal(GLOBAL_TRANSITION_STATE).isAvailable())
		{
			return is_session_transition_active(ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>(), when_almost_done);
		}
		return when_in_doubt;
	}

	bool is_session_transition_active(const TransitionState transition_state, bool when_almost_done) noexcept
	{
		switch (transition_state)
		{
		case TRANSITION_STATE_EMPTY:
		case TRANSITION_STATE_WAIT_HUD_EXIT:
		case TRANSITION_STATE_CONFIRM_FM_SESSION_JOINING:
		case TRANSITION_STATE_TERMINATE_MAINTRANSITION:
		case TRANSITION_STATE_DLC_INTRO_BINK:
			return false;

		case TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER:
		case TRANSITION_STATE_FM_SWOOP_DOWN:
			return when_almost_done;
		}
		return true;
	}

	bool is_session_or_transition_active(bool when_in_doubt) noexcept
	{
		return is_session_started() || is_session_transition_active(when_in_doubt);
	}

	bool is_session_freeroam() noexcept
	{
		if (!is_session_started())
		{
			return false;
		}

		return (*pointers::network_session)->session->rline_session.data.config.m_Attrs.m_SessionPurpose == 0;
	}
}
