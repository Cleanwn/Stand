#pragma once

#include "CommandSlider.hpp"

#include "AbstractEntity.hpp"
#include "eCommandApplyForceTypes.hpp"

namespace Stand
{
	class CommandSuperMelee : public CommandSlider
	{
	private:
		[[nodiscard]] static bool isPlayerVehicle(AbstractEntity& ent)
		{
			return ent.isVehicle() && ent.isAPlayerInAnySeat();
		}

		[[nodiscard]] static bool isLikelyImportantEntity(AbstractEntity& ent)
		{
			return ENTITY::IS_ENTITY_A_MISSION_ENTITY(ent) && HUD::GET_BLIP_FROM_ENTITY(ent) != 0;
		}

		void apply(AbstractEntity& ent)
		{
			v3 vec = g_player_ped.getPos().lookAt(ent.getPos()).toDirNoZ() * (value * 25.0f);

			ENTITY::APPLY_FORCE_TO_ENTITY(ent, APPLY_TYPE_ANGULAR_IMPULSE, 0.0f, 0.0f, 15.0f, 0, 0, 0, 0, true, false, true, false, true);
			ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(ent, APPLY_TYPE_IMPULSE, vec.x, vec.y, -1.0f, true, false, true, true);
		}

	public:
		explicit CommandSuperMelee(CommandList* const parent)
			: CommandSlider(parent, LOC("SPRMELEE"), CMDNAMES_OBF("supermelee"), LOC("SPRMELEE_H"), 0, 10, 0)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (num == 0 && allow_replacements)
			{
				return LANG_GET_W("DOFF");
			}

			return CommandSlider::formatNumber(num, allow_replacements);
		}

		void onChange(Click& click, int prev_value) final
		{
			if (value == 0)
			{
				return;
			}

			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(value != 0);

				std::vector<AbstractEntity> entities;
				
				AbstractEntity::getAllPeds(entities);
				AbstractEntity::getAllVehicles(entities);

				for (auto& ent : entities)
				{
					if (ent.isAPlayer())
					{
						continue;
					}

					if (ent.getPos().distance(g_player_ped.getPos()) <= 20.0f)
					{
						if (!isPlayerVehicle(ent) && !isLikelyImportantEntity(ent))
						{
							NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(ent);
						}

						if (WEAPON::HAS_ENTITY_BEEN_DAMAGED_BY_WEAPON(ent, 0, 1)
							&& ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ent, g_player_ped, TRUE)
							)
						{
							if (isPlayerVehicle(ent))
							{
								ent.requestControl(ATSTRINGHASH("CommandSuperMelee"), [=](AbstractEntity& ent)
								{
									apply(ent);
								});
							}
							else
							{
								apply(ent);
							}
						}

						WEAPON::CLEAR_ENTITY_LAST_WEAPON_DAMAGE(ent);
						ENTITY::CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ent);
					}
				}

				HANDLER_END;
			});
		}
	};
}