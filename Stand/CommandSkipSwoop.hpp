#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "natives.hpp"
#include "ScriptGlobal.hpp"
#include "TransitionState.hpp"

namespace Stand
{
	class CommandSkipSwoop : public CommandToggle
	{
	public:
		explicit CommandSkipSwoop(CommandList* const parent)
			: CommandToggle(parent, LOC("SSWPD"), { CMDNAME("skipswoopdown") }, LOC("SSWPD_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				if (*ScriptGlobal(GLOBAL_TRANSITION_STATE).as<TransitionState*>() == TRANSITION_STATE_FM_SWOOP_DOWN
					// This stuff goes kinda wrong with interiors, but it's hard to know if we're gonna spawn into an interior.
					// If we delay for the interior check, it becomes worse for the non-interior use case.
#if false
					&& STREAMING::GET_PLAYER_SWITCH_STATE() >= 6
					&& INTERIOR::GET_INTERIOR_FROM_ENTITY(g_player_ped) == 0
#endif
					)
				{
					STREAMING::STOP_PLAYER_SWITCH();
					ENTITY::FREEZE_ENTITY_POSITION(g_player_ped, false);
					PLAYER::SET_PLAYER_CONTROL(g_player, true, 0);
					NETWORK::SET_LOCAL_PLAYER_INVISIBLE_LOCALLY(false);
					NETWORK::SET_LOCAL_PLAYER_VISIBLE_LOCALLY(true);
					NETWORK::SET_PLAYER_INVISIBLE_LOCALLY(g_player, false);
					NETWORK::SET_PLAYER_VISIBLE_LOCALLY(g_player, true);
					HUD::DISPLAY_RADAR(true);
					HUD::DISPLAY_HUD(true);
				}
			});
		}
	};
}
