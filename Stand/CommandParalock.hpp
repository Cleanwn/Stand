#pragma once

#include "CommandListSelect.hpp"

#include "ePedConfigFlags.hpp"
#include "natives.hpp"
#include "tbTempEmptyParachute.hpp"

namespace Stand
{
	class CommandParalock : public CommandListSelect
	{
	public:
		explicit CommandParalock(CommandList* const parent)
			: CommandListSelect(parent, LOC("PARALCK"), { CMDNAME("paralock") }, NOLABEL, {
				{ 0, LOC("DOFF"), { CMDNAME("off"), CMDNAME("disabled") } },
				{ 1, LOC("LCK_FULL"), { CMDNAME("full") } },
				{ 2, LOC("LCK_ETY"), { CMDNAME("empty") } },
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			onChangeProcessScriptTickEventHandler(click, prev_value, 1,  []
			{
				if (!g_tb_temp_empty_parachute.isEnabled())
				{
					g_player_ped.ensureHasParachute();
					PLAYER::SET_PLAYER_HAS_RESERVE_PARACHUTE(g_player);
				}
			});
			onChangeProcessScriptTickEventHandler(click, prev_value, 2, []
			{
				WEAPON::REMOVE_WEAPON_FROM_PED(g_player_ped, ATSTRINGHASH("gadget_parachute"));

				// remove reserve
				if (auto cped = g_player_ped.getCPed())
				{
					PED::SET_PED_CONFIG_FLAG(g_player_ped, CPED_CONFIG_FLAG_HasReserveParachute, FALSE);
				}
			});
		}
	};
}
