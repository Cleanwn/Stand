#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandSubCarAutoSwitch : public CommandToggle
	{
	public:
		CommandToggle* config_noAnim;

		explicit CommandSubCarAutoSwitch(CommandList* const parent)
			: CommandToggle(parent, LOC("ATRNSUBCAR"))
		{
		}

		void onChange(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on)
				if (g_player_veh.isValid()
					&& g_player_veh.getModel().isSubmarineCar()
					)
				{
					if (VEHICLE::IS_VEHICLE_IN_SUBMARINE_MODE(g_player_veh))
					{
						if (ENTITY::GET_ENTITY_SUBMERGED_LEVEL(g_player_veh) <= 0.2f)
						{
							VEHICLE::TRANSFORM_TO_CAR(g_player_veh, config_noAnim->m_on);
						}
					}
					else
					{
						if (ENTITY::GET_ENTITY_SUBMERGED_LEVEL(g_player_veh) >= 0.8f)
						{
							VEHICLE::TRANSFORM_TO_SUBMARINE(g_player_veh, config_noAnim->m_on);
						}
					}
				}
				HANDLER_END
			});
		}
	};
}
