#pragma once

#include "CommandToggle.hpp"

#include "CommandDriveOnWater.hpp"
#include "gta_entity.hpp"

namespace Stand
{
	class CommandVehicleNoObjectCollision : public CommandToggle
	{
	public:
		explicit CommandVehicleNoObjectCollision(CommandList* const parent)
			: CommandToggle(parent, LOC("NOCOLOBJ"), CMDNAMES("noobjcollision", "noobjectcollision"), LOC("NOCOLOBJ_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				std::vector<AbstractEntity> objects{};

				AbstractEntity::getAllObjects([&objects](AbstractEntity&& obj)
				{
					if (CommandDriveOnWater::object != obj)
					{
						objects.emplace_back(std::move(obj));
					}

					CONSUMER_CONTINUE;
				});

				if (m_on)
				{
					if (g_player_veh.isValid() && g_player_veh.isOwnerOfVehicleAndDriver())
					{
						for (auto& ent : objects)
						{
							ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(ent, g_player_veh, TRUE);
						}
					}
				}
				else if (auto veh = Util::getVehicle(); veh.isValid()) // The game doesn't automatically reset collision for objects. We have to do it manually.
				{
					for (auto& ent : objects)
					{
						SOUP_IF_LIKELY (auto ptr = ent.getPointer())
						{
							SOUP_IF_LIKELY (auto component = ptr->m_DynamicEntityComponent)
							{
								if (component->m_pNoCollisionEntity == veh.getPointer())
								{
									component->m_pNoCollisionEntity = nullptr;
									component->m_nNoCollisionFlags = 0;
								}
							}
						}
					}
				}

				return m_on;
			});
		}
	};
}