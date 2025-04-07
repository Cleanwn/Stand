#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandGravityGun : public CommandToggle
	{
	private:
		CommandToggleNoCorrelation* const contrived_physics;
		CommandSlider* const force_multiplier;
		CommandToggleNoCorrelation* const collision;
		float distance_from_self;
		AbstractEntity target{};
		AbstractEntity yeeted_target{};

	public:
		explicit CommandGravityGun(CommandList* parent, CommandToggleNoCorrelation* contrived_physics, CommandSlider* force_multiplier, CommandToggleNoCorrelation* collision)
			: CommandToggle(parent, LOC("GRTYGUN"), { CMDNAME("gravitygun") }), contrived_physics(contrived_physics), force_multiplier(force_multiplier), collision(collision)
		{
		}

	private:
		void releaseTarget()
		{
			ENTITY::SET_ENTITY_COLLISION(target, true, false);
			ENTITY::SET_ENTITY_HAS_GRAVITY(target, true);
			target.reset();
		}

	public:
		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (target.isValid())
				{
					if (!target.exists())
					{
						target.reset();
					}
					else if (!m_on)
					{
						releaseTarget();
					}
					else if (Input::isCommandInputAllowed() && Input::isPressingAttack())
					{
						auto momentum = CAM::GET_FINAL_RENDERED_CAM_ROT(2).toDir();
						momentum *= 1000.0f;
						momentum *= (float(force_multiplier->value) / 100.0f);
						ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(target, 1, momentum.x, momentum.y, momentum.z, true, false, true, true);
						yeeted_target = target;
						releaseTarget();
					}
					else
					{
						if (PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_CURSOR_SCROLL_UP))
						{
							distance_from_self += 1.0f;
						}
						if (PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_CURSOR_SCROLL_DOWN))
						{
							distance_from_self -= 1.0f;
						}
						auto pos = target.getPos();
						auto desired_pos = (g_cam_ped.getPos() + (CAM::GET_FINAL_RENDERED_CAM_ROT(2).toDir() * (distance_from_self + target.getDimensions().max())));
						auto momentum = pos.lookAt(desired_pos).toDir();
						//float distance = pos.distanceSquared(desired_pos);
						if (Input::isPressingAim())
						{
							if (collision->m_on)
							{
								ENTITY::SET_ENTITY_COLLISION(target, true, false);
							}
							else
							{
								ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(target, false, false);
							}
							ENTITY::SET_ENTITY_HAS_GRAVITY(target, false);
							if (!target.isOwner())
							{
								NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(target);
							}
							if (contrived_physics->m_on)
							{
								momentum *= (pos.distance(desired_pos) / 8.94427191f);
								momentum *= (float(force_multiplier->value) / 100.0f);
								pos += momentum;
								target.setPos(pos);
							}
							else
							{
								momentum *= (pos.distance(desired_pos) / 5.47722557505f);
								momentum *= (float(force_multiplier->value) / 100.0f);
								ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(target, 1, momentum.x, momentum.y, momentum.z, true, false, true, true);
							}
						}
						else
						{
							//if (distance > 10.0f)
							{
								momentum *= 10.0f;
								momentum *= (float(force_multiplier->value) / 100.0f);
								ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(target, 1, momentum.x, momentum.y, momentum.z, true, false, true, true);
							}
							releaseTarget();
						}
					}
				}
				else if(m_on)
				{
					Entity _ent;
					if (Input::isPressingAim())
					{
						if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(g_player, &_ent))
						{
							target = AbstractEntity::get(_ent);
							if (target.isPed())
							{
								Vehicle veh = target.getVehicle(false);
								if (veh != 0)
								{
									target = AbstractEntity::get(veh);
									distance_from_self = 10.0f;
								}
							}
							if (target.isAPlayer()
								|| (yeeted_target.exists() && target == yeeted_target))
							{
								target.reset();
							}
						}
					}
					else
					{
						yeeted_target.reset();
					}
				}
				return m_on;
			});
		}
	};
}
