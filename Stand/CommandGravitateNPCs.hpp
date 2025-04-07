#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractEntity.hpp"
#include "AttackerMgr.hpp"
#include "BodyguardMgr.hpp"

#ifdef STAND_DEBUG
#include "drawDebugText.hpp"
#endif

namespace Stand
{
	class CommandGravitateNPCs : public CommandPlayerToggle
	{
	private:
		static inline constexpr uint8_t max_peds = 10;
		static inline constexpr uint8_t max_vehs = 40;

		static void collectEntities(std::vector<AbstractEntity>& entities)
		{
			uint8_t peds = 0;
			uint8_t vehs = 0;

			AbstractEntity::getAllPeds([&entities, &peds](AbstractEntity&& entity)
			{
				if (++peds < max_peds)
				{
					entities.emplace_back(std::move(entity));
					CONSUMER_CONTINUE;
				}

				CONSUMER_BREAK;
			});

			AbstractEntity::getAllVehicles([&entities, &vehs](AbstractEntity&& entity)
			{
				if (++vehs < max_vehs)
				{
					entities.emplace_back(std::move(entity));
					CONSUMER_CONTINUE;
				}

				CONSUMER_BREAK;
			});
		}

		[[nodiscard]] static bool isEntityAcceptable(AbstractEntity& entity)
		{
			if (!entity.exists()
				|| AttackerMgr::isEntityAttacker(entity)
				|| ENTITY::IS_ENTITY_A_MISSION_ENTITY(entity)
				)
			{
				return false;
			}
			else if (entity.isPed())
			{
				return !entity.isAPlayer() && !entity.isInAnyVehicle() && !BodyguardMgr::isPedBodyguard(entity);
			}
			else if (entity.isVehicle())
			{
				return !entity.isAPlayerInAnySeat();
			}

			return false;
		}

	public:
		explicit CommandGravitateNPCs(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PLYGRVNPC"), CMDNAMES_OBF("gravitate"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (auto ped = AbstractPlayer(getPlayer()).getPed(); ped.isValid())
				{
					std::vector<AbstractEntity> entities;
					entities.reserve((size_t)(max_peds + max_vehs));
					
					collectEntities(entities);

					for (auto& entity : entities)
					{
						if (!isEntityAcceptable(entity)) // Doing this inside of the collectors would be nice, so 100% of 'entities' are entities we'll use. However, it doubles the performance loss of this feature.
						{
							continue;
						}
						
						if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(entity))
						{
							if (m_on
								&& entity.isPed()
								&& !PED::IS_PED_RAGDOLL(ped)
								)
							{
								PED::SET_PED_TO_RAGDOLL(entity, 1000, 0, 0, FALSE, FALSE, FALSE);
							}

							entity.setCanMigrate(!m_on);
							
							v3 pos;

							if (m_on)
							{
								pos = entity.getPos() - ped.getPos();
								pos.normalise();
								pos -= (pos + pos);
							}
							else
							{
								pos = ENTITY::GET_ENTITY_FORWARD_VECTOR(entity) * 1000.0f;
							}

							ENTITY::APPLY_FORCE_TO_ENTITY(entity, 3, pos.x, pos.y, pos.z, 0.0f, 0.0f, 2.0f, 0, FALSE, FALSE, TRUE, FALSE, FALSE);
						}
						else
						{
							NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity);
						}
					}
				}

				return m_on;
			});
		}

		void onDisable(Click& click) final
		{
			ensureScriptThread([]
			{
				std::vector<AbstractEntity> entities;

				AbstractEntity::getAllPeds(entities);
				AbstractEntity::getAllVehicles(entities);

				for (auto& e : entities)
				{
					if (isEntityAcceptable(e))
					{
						e.setCanMigrate(true);
					}
				}
			});
		}
	};
}