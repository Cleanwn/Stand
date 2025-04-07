#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandAirSwim : public CommandToggle
	{
	public:
		explicit CommandAirSwim(CommandList* const parent)
			: CommandToggle(parent, LOC("AIRSWM"), CMDNAMES("swiminair", "airswim"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				PED::SET_PED_CONFIG_FLAG(g_player_ped, 65, TRUE);
			});
		}
	};
}
