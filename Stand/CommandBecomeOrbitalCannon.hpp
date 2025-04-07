#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandBecomeOrbitalCannon : public CommandToggle
	{
	public:
		explicit CommandBecomeOrbitalCannon(CommandList* const parent)
			: CommandToggle(parent, LOC("BEORB"), CMDNAMES("becomeorbitalcannon", "beorbitalcannon"), LOC("BEORB_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (*pointers::is_session_started)
				{
					*ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(g_player, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_BITSET1).as<int*>() |= (1 << GLOBAL_PLAYERSTATUS2_BITSET1_ORBITALKIL);
				}
			});
			if (!m_on)
			{
				if (*pointers::is_session_started)
				{
					ensureScriptThread(click, []
					{
						*ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(g_player, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_BITSET1).as<int*>() &= ~(1 << GLOBAL_PLAYERSTATUS2_BITSET1_ORBITALKIL);
					});
				}
			}
		}
	};
}
