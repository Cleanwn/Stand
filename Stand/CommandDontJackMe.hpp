#pragma once

#include "CommandToggle.hpp"

#include "ePedConfigFlags.hpp"

namespace Stand
{
	class CommandDontJackMe : public CommandToggle
	{
	public:
		explicit CommandDontJackMe(CommandList* const parent)
			: CommandToggle(parent, LOC("NOJACK"), CMDNAMES("nojacking", "nohijacking"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				g_player_ped.setConfigFlag(CPED_CONFIG_FLAG_DontDragMeOutCar, true);
			});
			if (!m_on)
			{
				g_player_ped.setConfigFlag(CPED_CONFIG_FLAG_DontDragMeOutCar, false);
			}
		}
	};
}
