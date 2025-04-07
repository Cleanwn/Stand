#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandVehicleNoPedCollision : public CommandToggle
	{
	public:
		explicit CommandVehicleNoPedCollision(CommandList* const parent)
			: CommandToggle(parent, LOC("NOCOLPED"), CMDNAMES("nopedcollision"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);

				if (g_player_veh.isValid() && g_player_veh.isOwnerOfVehicleAndDriver())
				{
					AbstractEntity::getAllPeds([](AbstractEntity&& ent)
					{
						if (ent != g_player_ped)
						{
							ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(ent, g_player_veh, TRUE);
						}

						CONSUMER_CONTINUE;
					});
				}

				HANDLER_END;
			});
		}
	};
}