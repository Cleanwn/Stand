#pragma once

#include "CommandListSelect.hpp"

#include "is_session.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandVehInvisibility : public CommandListSelect
	{
	public:
		inline static long long player_ped_invisiblity = 0;

		explicit CommandVehInvisibility(CommandList* const parent)
			: CommandListSelect(parent, LOC("INVIS"), { CMDNAME("vehinvisibility") }, NOLABEL, {
				{0, LOC("DOFF"), CMDNAMES("off")},
				{1, LOC("LOCVIS"), CMDNAMES("remote")},
				{2, LOC("ON"), CMDNAMES("on")}
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (value == 1)
			{
				registerScriptTickEventHandler(click, [this]
				{
					const bool on = (value == 1);
					if (is_session_started())
					{
						const Vehicle veh = Util::getVehicle();
						if (veh != 0)
						{
							ENTITY::SET_ENTITY_VISIBLE(veh, !on, false);
							NETWORK::SET_ENTITY_LOCALLY_VISIBLE(veh);
							if (player_ped_invisiblity == 2)
							{
								NETWORK::SET_ENTITY_LOCALLY_INVISIBLE(g_player_ped);
							}
						}
					}
					return on;
				});
			}
			else if (value == 2)
			{
				registerScriptTickEventHandler(click, [this]
				{
					const bool on = (value == 2);
					const Vehicle veh = Util::getVehicle();
					if (veh != 0)
					{
						ENTITY::SET_ENTITY_VISIBLE(veh, !on, false);
						if (on && player_ped_invisiblity == 0)
						{
							ENTITY::SET_ENTITY_VISIBLE(g_player_ped, true, false);
						}
					}
					return on;
				});
			}
		}
	};
}
