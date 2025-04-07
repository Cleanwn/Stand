#pragma once

#include "CommandPausable.hpp"

#include "is_session.hpp"
#include "natives.hpp"
#include "paused.hpp"

namespace Stand
{
	class CommandPausable : public CommandToggle
	{
	public:
		explicit CommandPausable(CommandList* const parent)
			: CommandToggle(parent, LOC("PAWS"), { CMDNAME("pausable") })
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				g_paused = HUD::IS_PAUSE_MENU_ACTIVE()
					&& SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(ATSTRINGHASH("pausemenu_multiplayer")) == 0
					&& !is_session_transition_active(true, true)
					&& !NETWORK::NETWORK_IS_IN_TRANSITION()
					&& !g_hooking.managed_join // Fix for Friend Method
					;
				MISC::SET_GAME_PAUSED((BOOL)g_paused);
			});
			if (!m_on)
			{
				MISC::SET_GAME_PAUSED(FALSE);
			}
		}
	};
}
