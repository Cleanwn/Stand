#include "AllEntitiesEveryTick.hpp"

#include "AbstractPlayer.hpp"
#include "Box.hpp"
#include "Camgr.hpp"
#include "CommandToggleRegularSp.hpp"
#include "DrawUtil3d.hpp"
#include "dedsec_hacks.hpp"
#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"
#include "gta_entity.hpp"
#include "gta_input.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "huddecl.hpp"
#include "input.hpp"
#include "is_session.hpp"
#include "joaatToString.hpp"
#include "FileLogger.hpp"
#include "natives.hpp"
#include "PedBoneId.hpp"
#include "punishments.hpp"
#include "Renderer.hpp"
#include "pointers.hpp"
#include "tbReticle.hpp"
#include "tbScreenshotMode.hpp"
#include "ScriptGlobal.hpp"
#include "tbFreecam.hpp"
#include "TransitionHelper.hpp"
#include "unrestrictGameplayCam.hpp"
#include "Util.hpp"

//#include "CPickup.hpp"
//#include "joaatToString.hpp"
//#include "StringUtils.hpp"

#define INSERT_HACK(hack) if (hack.enabled) hacks.emplace_back(&hack)

namespace Stand
{
	[[nodiscard]] static std::string getDedsecTutorialMessage(int id)
	{
		switch (id)
		{
		case 0:
			return LANG_GET("DEDSEC_TUT_ESP");

		case 1:
			{
				std::string help_text = LANG_GET("DEDSEC_TUT_OPT");
				if (Input::keyboard_and_mouse)
				{
					help_text = fmt::format(fmt::runtime(help_text), LANG_GET("DEDSEC_TUT_OPTMSE"));
				}
				return fmt::format(fmt::runtime(help_text), "~INPUT_PHONE~");
			}

		case 2:
			return LANG_GET("DEDSEC_TUT_SEL");
		}
		return {};
	}

	static bool showDedsecTutorialMessage(int id)
	{
		if (HUD::IS_HELP_MESSAGE_BEING_DISPLAYED())
		{
			bool is_dedsec_tutorial_message = false;
			for (int i = 0; i <= 2; i++)
			{
				if (i == id)
				{
					continue;
				}
				if (Util::is_help_message_being_displayed(getDedsecTutorialMessage(i)))
				{
					is_dedsec_tutorial_message = true;
					break;
				}
			}
			if (!is_dedsec_tutorial_message)
			{
				return false;
			}
		}
		Util::show_corner_help_impl(getDedsecTutorialMessage(id), true);
		return true;
	}

	void AllEntitiesEveryTick::onTick()
	{
		/*{
			std::vector<AbstractEntity> objects{};
			AbstractEntity::getAllObjects(objects);
			for (AbstractEntity obj : objects)
			{
				g_renderer.draw3dTextThisTickH(obj.getPos(), StringUtils::utf8_to_utf16(joaatToString(obj.getModelHash())), 1.0f);
			}
		}*/
		/*{
			std::vector<AbstractEntity> objects{};
			AbstractEntity::getAllPickups(objects);
			for (AbstractEntity obj : objects)
			{
				g_renderer.draw3dTextThisTickH(obj.getPos(), StringUtils::utf8_to_utf16(
					fmt::format("{} ({})", joaatToString(obj.getModelHash()), joaatToString(static_cast<CPickup*>(obj.getPointer())->m_pickupHash))
				), 1.0f);
			}
		}*/

		if (g_hooking.erase_objects_with_no_mi)
		{
			onTickEraseObjectsWithNoModelInfo();
		}

		const bool punishments_this_tick = (npc_existence_punishments != 0 || npc_proximity_punishments != 0 || npc_aim_punishments != 0 || npc_hostility_punishments != 0 || player_aim_punishments != 0);

		bool dedsec_find_target_this_tick = false;
		bool dedsec_should_show_reticle = false;
		AbstractEntity dedsec_target;
		if (dedsec_mode
			&& g_renderer.gameplayState == PLAYING // not doesGameplayStateAllowMenuCommandInput to avoid issues with interaction menu
			&& !(g_gui.isInMenuNavigationContext() && Input::doesSchemeConflictWithInput(INPUT_PHONE))
			&& CommandToggleRegularSp::checkPermissions()
			&& (CAM::IS_GAMEPLAY_CAM_RENDERING() || g_tb_freecam.isEnabled()) // Prevent dedsec from interrupting store menu navigation (LSC, clothes, likely others).
			)
		{
			if (dedsec_selected_target.isValid() && !dedsec_selected_target.exists())
			{
				dedsec_selected_target.reset();
			}
			if (dedsec_selected_target.isValid())
			{
				dedsec_target = dedsec_selected_target;
				dedsec_should_show_reticle = dedsec_active_reticle;
			}
			else
			{
				dedsec_find_target_this_tick = true;
				dedsec_should_show_reticle = (dedsec_passive_reticle && !g_tb_screenshot_mode.isEnabled());
			}
		}
		else
		{
			if (dedsec_selected_target.isValid())
			{
				dedsec_selected_target.reset();
			}
		}

		if (dedsec_should_show_reticle)
		{
			if (!dedsec_enabled_reticle)
			{
				dedsec_enabled_reticle = true;
				g_tb_reticle.enable();
			}
		}
		else
		{
			if (dedsec_enabled_reticle)
			{
				dedsec_enabled_reticle = false;
				g_tb_reticle.disable();
			}
		}

		bool aimbot_find_target_this_tick = false;
		if (aimbot)
		{
			if (g_player_ped.isAiming())
			{
				if (aimbot_target.isValid() && (!aimbot_target.exists() || aimbot_target.isDead() || !aimbot_target.isInLineOfSight()))
				{
					aimbot_target.reset();
				}
				if (!aimbot_target.isValid())
				{
					aimbot_find_target_this_tick = true;
				}
			}
			else
			{
				aimbotDisable();
			}
		}

		const bool no_collision_with_other_vehicles_this_tick = g_gui.no_collision_with_other_vehicles && g_player_veh.isValid() && g_player_veh.isOwnerOfVehicleAndDriver();

		if (dedsec_find_target_this_tick || punishments_this_tick || aimbot_find_target_this_tick || g_gui.second_person_cam || no_collision_with_other_vehicles_this_tick)
		{
			std::vector<AbstractEntity> aimbot_entities;
			std::vector<AbstractEntity> dedsec_entities;
			v3 nearest_ped_head_pos;
			float nearest_ped_distance = FLT_MAX;

			Vehicle nearest_vehicle_with_trailer;
			float nearest_vehicle_with_trailer_distance = FLT_MAX;
			AbstractEntity g_player_veh_trailer = AbstractEntity::invalid();
			if (no_collision_with_other_vehicles_this_tick)
			{
				Vehicle trailer = 0;
				VEHICLE::GET_VEHICLE_TRAILER_VEHICLE(g_player_veh, &trailer);
				if (trailer != 0)
				{
					g_player_veh_trailer = AbstractEntity::get(trailer);
				}
			}

			AbstractEntity::getAllVehicles([&](AbstractEntity&& ent)
			{
				if (dedsec_find_target_this_tick && ent != g_cam_veh && ent != g_preview_ent)
				{
					dedsec_entities.emplace_back(ent);
				}
				if (no_collision_with_other_vehicles_this_tick && ent != g_player_veh && ent != g_player_veh_trailer)
				{
					// An entity can only have one "no collision entity" so if we change it for a vehicle with a trailer,
					// it will go crazy as you'd expect when an attachment has collision with its "parent."
					// To fix this, we're using our own vehicle's sole "no collision entity" to be whichever vehicle with a trailer is closest to us.
					// Now, when our own vehicle has a trailer as well, we can't do that, so we will have collision with other "trailered" vehicles,
					// but we can at least use our trailer's sole "no collision entity" to be the closest "untrailered" vehicle to not break the illusion so easily.
					float dist = FLT_MAX;
					if (VEHICLE::IS_VEHICLE_ATTACHED_TO_TRAILER(ent))
					{
						if (!g_player_veh_trailer.isValid())
						{
							dist = g_player_veh.getPos().distance(ent.getPos());
						}
					}
					else
					{
						ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(ent, g_player_veh, true);
						if (g_player_veh_trailer.isValid())
						{
							dist = g_player_veh_trailer.getPos().distance(ent.getPos());
						}
					}
					if (dist < nearest_vehicle_with_trailer_distance)
					{
						nearest_vehicle_with_trailer = ent;
						nearest_vehicle_with_trailer_distance = dist;
					}
				}
				if (aimbot_find_target_this_tick)
				{
					if (ent.canTarget(aimbot_targeting, false))
					{
						aimbot_entities.emplace_back(ent);
					}
				}
				CONSUMER_CONTINUE;
			});
			AbstractEntity::getAllPeds([&](AbstractEntity&& ent)
			{
				if (ent == g_player_ped || ent == g_cam_ped)
				{
					CONSUMER_CONTINUE;
				}
				//g_renderer.draw3dTextThisTickH(ent.getPos(), StringUtils::utf8_to_utf16(fmt::to_string((void*)ent.getPointer())), 1.0f);
				if (g_gui.second_person_cam)
				{
					const v3 pos = ent.getHeadPos();
					const float ped_distance = pos.distance(g_player_ped.getPos());
					if (ped_distance < nearest_ped_distance)
					{
						nearest_ped_head_pos = pos;
						nearest_ped_distance = ped_distance;
					}
				}
				if (dedsec_find_target_this_tick && !ent.isInAnyVehicle() && ent != g_preview_ent)
				{
					dedsec_entities.emplace_back(ent);
				}
				if ((!punishments_this_tick && !aimbot_find_target_this_tick) || (!ent.couldBePlayer() && ent.isFriend()))
				{
					CONSUMER_CONTINUE;
				}
				if (aimbot_find_target_this_tick)
				{
					if (ent.canTarget(aimbot_targeting, false))
					{
						aimbot_entities.emplace_back(ent);
					}
					if (!punishments_this_tick)
					{
						CONSUMER_CONTINUE;
					}
				}
				punishment_t punishments;
				PunishmentTrigger punishment_trigger = PunishmentTrigger::EXISTENCE;
				if (ent.couldBePlayer())
				{
					if (auto player = ent.getPlayer(); player.isValid() && player.exists())
					{
						if (player_aim_excludes.isExcluded(player))
						{
							CONSUMER_CONTINUE;
						}
					}
					punishments = 0;
				}
				else
				{
					punishments = npc_existence_punishments;
					if (ent.getPos().distance(g_player_ped.getPos()) < npc_punishable_proximity)
					{
						punishments |= npc_proximity_punishments;
						punishment_trigger = PunishmentTrigger::PROXMITY;
					}

					const auto combat_target = PED::GET_PED_TARGET_FROM_COMBAT_PED(ent, FALSE);
					const bool use_any_target_for_hostility_check = AllEntitiesEveryTick::npc_hostility_include_everyone || (AllEntitiesEveryTick::npc_hostility_include_everyone_in_missions && !is_session_freeroam());

					if (use_any_target_for_hostility_check && combat_target > 0)
					{
						punishments |= npc_hostility_punishments;
						punishment_trigger = PunishmentTrigger::HOSTILE;
					}
					else if (!is_session_transition_active(true))
					{
						for (auto& player : AbstractPlayer::listAll())
						{
							if (auto entity = player.getPed(); entity.isValid())
							{
								if (player == g_player
									|| player.satisfiesAnyOf(AllEntitiesEveryTick::npc_hostility_include_friends, AllEntitiesEveryTick::npc_hostility_include_crew, false, AllEntitiesEveryTick::npc_hostility_include_org)
									|| (AllEntitiesEveryTick::npc_hostility_include_passengers && g_player_veh.isValid() && entity.getVehicle() == g_player_veh) // Also includes if they are driver; intentional.
									)
								{
									if (combat_target == entity.getHandle())
									{
										punishments |= npc_hostility_punishments;
										punishment_trigger = PunishmentTrigger::HOSTILE;
										break;
									}
								}
							}
						}
					}

					if (punishments != 0
						&& (is_session_transition_active(true, true)
							|| GET_MILLIS_SINCE(TransitionHelper::ts_started) < 5000
							)
						)
					{
						punishments = 0;
					}
				}
				/*if (util::VDIST(util::GET_ENTITY_COORDS(g_player_ped), util::GET_ENTITY_COORDS(ped)) <= m_character_scale_multiplier)
				{
					punishments |= PUNISHMENT_DIE;
				}*/
				bool is_aiming = ent.isAiming() && g_player_ent.isInLineOfSight(ent.getBonePos(PedBoneId::SKEL_Head));
				if (is_aiming && (!(ent.couldBePlayer() ? player_needs_to_aim_at_user : npc_needs_to_aim_at_user) || PED::IS_PED_FACING_PED(ent, g_player_ped, ent.getPos().fov_to(g_player_ped.getPos()))))
				{
					punishments |= ent.couldBePlayer() ? player_aim_punishments : npc_aim_punishments;
					punishment_trigger = PunishmentTrigger::AIMED_AT;
				}
				/*if (punishments != 0 && ent.isPlayer())
				{
					g_logger.log(fmt::format("Punishing {}", ent.get_player().get_name()));
				}*/
				Punishments::apply(punishments, ent, punishment_trigger);
				CONSUMER_CONTINUE;
			});

			if (g_gui.second_person_cam)
			{
				if (nearest_ped_distance == FLT_MAX)
				{
					Camgr::unlock_pos();
					Camgr::unlock_rot();
				}
				else
				{
					Camgr::lock_pos(nearest_ped_head_pos);
					Camgr::lock_rot(nearest_ped_head_pos.lookAt(g_player_ped.getPos()));
				}
			}

			if (dedsec_find_target_this_tick)
			{
				auto ent = AbstractEntity::getNearestVisible(dedsec_entities);
				if (ent.isValid())
				{
					dedsec_target = ent;
				}
				else if (dedsec_tutorial_state == DEDSEC_TUTORIAL_STATE_TARGET)
				{
					showDedsecTutorialMessage(0);
				}
			}

			if (aimbot_find_target_this_tick)
			{
				aimbot_target = AbstractEntity::getNearestVisible(aimbot_entities, aimbot_los_check);
			}

			if (nearest_vehicle_with_trailer_distance != FLT_MAX)
			{
				if (g_player_veh_trailer.isValid())
				{
					ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(g_player_veh_trailer, nearest_vehicle_with_trailer, true);
				}
				else
				{
					ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(g_player_veh, nearest_vehicle_with_trailer, true);
				}
			}
		}

		if (aimbot)
		{
			if (g_player_ped.isAiming())
			{
				if (aimbot_target_box)
				{
					aimbot_target.getBox().drawLines((int)(g_renderer.arColour.R() * 255.0f), (int)(g_renderer.arColour.G() * 255.0f), (int)(g_renderer.arColour.B() * 255.0f), (int)(g_renderer.arColour.A() * 255.0f));
				}

				if (!aimbot_captured_rot)
				{
					aimbot_captured_rot = true;
					aimbot_rot = CAM::GET_GAMEPLAY_CAM_ROT(0);
					Camgr::lock_rot(aimbot_rot);
				}

				if (Input::isPressingAttack())
				{
					aimbot_last_attack = get_current_time_millis();
				}

				v3 cur_rot = CAM::GET_GAMEPLAY_CAM_ROT(0);
				if (aimbot_rot != cur_rot)
				{
					Camgr::lock_rot(CAM::GET_FINAL_RENDERED_CAM_ROT(0) + (cur_rot - aimbot_rot));

					// heuristic to determine if user moved the cam
					if (cur_rot.distance(aimbot_rot) > (GET_MILLIS_SINCE(aimbot_last_attack) > 1000 ? 0.02f : 1.f))
					{
						aimbot_lerping = 1.0f;
						aimbot_last_move = get_current_time_millis();
						aimbot_target.reset();
					}

					aimbot_rot = cur_rot;
				}

				//drawDebugText(GET_MILLIS_SINCE(last_move));

				if (GET_MILLIS_SINCE(aimbot_last_move) > aimbot_lockout
					&& aimbot_target.isValid()
					)
				{
					const Vector3 cam_pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
					v3 head_pos = aimbot_target.isPed() ? aimbot_target.getBonePos(aimbot_ped_bone) : aimbot_target.getPos();

					float tick_buffer = 1.0f;
					if (aimbot_smoothing != 0)
					{
						tick_buffer = (float)aimbot_smoothing;
					}
					head_pos += (ENTITY::GET_ENTITY_VELOCITY(aimbot_target) * (tick_buffer * 0.022f));

					//DrawUtil3d::draw_line(aimbot_target.getPos(), head_pos);

					if (aimbot_lerping == 1.0f)
					{
						aimbot_lerping = 0.0f;
						aimbot_lerp_from = soup::Quaternion::fromEulerXYZ(CAM::GET_FINAL_RENDERED_CAM_ROT(0));
						aimbot_lerp_to = soup::Quaternion::fromEulerXYZ(cam_pos.lookAt(head_pos));
					}
					else
					{
						aimbot_lerping += (1.0f / aimbot_smoothing);
						if (aimbot_lerping >= 1.0f)
						{
							aimbot_lerping = 1.0f;
							Camgr::lock_rot(aimbot_lerp_to);
						}
						else
						{
							Camgr::lock_rot(soup::Quaternion::slerp(aimbot_lerp_from, aimbot_lerp_to, aimbot_lerping, true));
						}
					}

					if (Input::isPressingAttack() && aimbot_magic_bullets)
					{
						const auto weapon = WEAPON::GET_SELECTED_PED_WEAPON(g_player_ped);
						const auto damage = WEAPON::GET_WEAPON_DAMAGE(weapon, 0);

						aimbot_target.sendMagicBullet(weapon, damage, false);
					}
				}

				if (CAM::GET_CAM_VIEW_MODE_FOR_CONTEXT(CAM::GET_CAM_ACTIVE_VIEW_MODE_CONTEXT()) == 4) // first person
				{
					NETWORK::SET_ENTITY_LOCALLY_INVISIBLE(g_player_ped);
				}
				else
				{
					Hash weapon;
					WEAPON::GET_CURRENT_PED_WEAPON(g_player_ped, &weapon, 1);
					if (WEAPON::GET_WEAPONTYPE_GROUP(weapon) == -1212426201) // sniper
					{
						NETWORK::SET_ENTITY_LOCALLY_INVISIBLE(g_player_ped);
					}
				}
			}
			else
			{
				aimbotEnsureCamIsFree();
			}
		}

		if (g_gui.isUnloadPending())
		{
			*ScriptGlobal(GLOBAL_INPUT_PHONE).as<int*>() = INPUT_PHONE;
		}
		else if (g_gui.isInMenuNavigationContext()
			&& Input::doesSchemeConflictWithInput(INPUT_PHONE)
			)
		{
			*ScriptGlobal(GLOBAL_INPUT_PHONE).as<int*>() = -1;
			phone_input_start = 0;
		}
		else if (dedsec_target.isValid())
		{
			if (phone_input_start == 0)
			{
				*ScriptGlobal(GLOBAL_INPUT_PHONE).as<int*>() = -1;
				if (PAD::IS_DISABLED_CONTROL_PRESSED(2, INPUT_PHONE))
				{
					phone_input_start = get_current_time_millis();
				}
			}
			else if (phone_input_start == -1)
			{
				phone_input_start = 0;
			}
			else
			{
				if (!PAD::IS_DISABLED_CONTROL_PRESSED(2, INPUT_PHONE))
				{
					//Util::toast(LIT(fmt::to_string(GET_MILLIS_SINCE(phone_input_start))), TC_SCRIPT_NOYIELD);
					if (GET_MILLIS_SINCE(phone_input_start) < phone_input_delay)
					{
						*ScriptGlobal(GLOBAL_INPUT_PHONE).as<int*>() = INPUT_PHONE;
						PAD::SET_CONTROL_VALUE_NEXT_FRAME(2, INPUT_PHONE, 1.0f);
						phone_input_start = -1;
					}
					else
					{
						*ScriptGlobal(GLOBAL_INPUT_PHONE).as<int*>() = -1;
						phone_input_start = 0;
					}
				}
			}
		}
		else
		{
			*ScriptGlobal(GLOBAL_INPUT_PHONE).as<int*>() = INPUT_PHONE;
			phone_input_start = 0;
		}

		if (dedsec_target.isValid())
		{
			if (dedsec_selected_target.isValid())
			{
				if (dedsec_active_line)
				{
					DrawUtil3d::draw_focus_line(dedsec_target.getPos(), Renderer::convert_to_dx_colour(dedsec_r, dedsec_g, dedsec_b, dedsec_a));
				}
				if (dedsec_active_box)
				{
					dedsec_target.getBox().drawLines(dedsec_r, dedsec_g, dedsec_b, dedsec_a);
				}

				if (dedsec_tutorial_state == DEDSEC_TUTORIAL_STATE_OPTION)
				{
					if (showDedsecTutorialMessage(2))
					{
						dedsec_tutorial_state = DEDSEC_TUTORIAL_STATE_DONE;
					}
				}
				const Vector3 cam_rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
				float cam_offset_x;
				{
				redo_x:
					cam_offset_x = ((cam_rot.z - dedsec_selected_target_start_rot.z) / 160.0f);
					if (cam_offset_x > (192.0f * HUD_X))
					{
						if (cam_offset_x > 180.0f / 160.0f)
						{
							dedsec_selected_target_start_rot.z += 360.0f;
						}
						else
						{
							dedsec_selected_target_start_rot.z += HUD_X * 160.0f;
						}
						goto redo_x;
					}
					if (cam_offset_x < (-192.0f * HUD_X))
					{
						if (cam_offset_x < -180.0f / 160.0f)
						{
							dedsec_selected_target_start_rot.z -= 360.0f;
						}
						else
						{
							dedsec_selected_target_start_rot.z -= HUD_X * 160.0f;
						}
						goto redo_x;
					}
				}
				float cam_offset_y;
				{
				redo_y:
					cam_offset_y = ((cam_rot.x - dedsec_selected_target_start_rot.x) / 90.0f);
					if (cam_offset_y > (192.0f * HUD_Y))
					{
						dedsec_selected_target_start_rot.x += HUD_Y * 90.0f;
						goto redo_y;
					}
					if (cam_offset_y < (-192.0f * HUD_Y))
					{
						dedsec_selected_target_start_rot.x -= HUD_Y * 90.0f;
						goto redo_y;
					}
				}
				std::vector<DedsecHack*> hacks = {};
				if (dedsec_selected_target.isFrozen())
				{
					INSERT_HACK(DedsecHack::unfreeze);
				}
				else
				{
					INSERT_HACK(DedsecHack::freeze);
				}
				if (dedsec_selected_target.isVehicle())
				{
					INSERT_HACK(DedsecHack::destroy);
					INSERT_HACK(DedsecHack::drive);
					INSERT_HACK(DedsecHack::enter);
					INSERT_HACK(DedsecHack::empty);
					INSERT_HACK(DedsecHack::ignite);
					INSERT_HACK(DedsecHack::slingshot);
					if (dedsec_selected_target.getDriver().isAPlayer())
					{
						INSERT_HACK(DedsecHack::menu_player_veh);
					}
				}
				else // is ped
				{
					INSERT_HACK(DedsecHack::explode);
					INSERT_HACK(DedsecHack::disarm);
					if (dedsec_selected_target.isAPlayer())
					{
						INSERT_HACK(DedsecHack::kill);
						INSERT_HACK(DedsecHack::cage);
						INSERT_HACK(DedsecHack::menu_player);
						goto dedsec_player_skip;
					}
					if (dedsec_selected_target.isDead())
					{
						INSERT_HACK(DedsecHack::revive);
					}
					else
					{
						INSERT_HACK(DedsecHack::burn);
					}
					INSERT_HACK(DedsecHack::cower);
					INSERT_HACK(DedsecHack::flee);
				}
				INSERT_HACK(DedsecHack::del);
			dedsec_player_skip:
				// Calculate x & y
				{
					float i = 0.0f;
					const float step_size = DEG_TO_RAD(360.0f / float(hacks.size()));
					for (DedsecHack* hack : hacks)
					{
						const auto n1 = hacks.size() >= 9 ? 125.0f : 96.0f;
						const auto n2 = !(hacks.size() >= 9) ? 0.0f : 10.0f;
						hack->x = ((-sin(i) * n1) * float(HUD_X));
						hack->y = ((cos(i) * (n1 - n2)) * float(HUD_Y));
						i += step_size;
					}
				}
				// Rendering & Collision
				const bool just_released = PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, INPUT_PHONE);
				bool any_active = false;
				for (const DedsecHack* hack : hacks)
				{
					const float offset_x = hack->x - cam_offset_x;
					const float offset_y = hack->y - cam_offset_y;					
					bool active = (32.0f * (float)HUD_X) > offset_x && offset_x > (-32.0f * (float)HUD_X) && (32.0f * (float)HUD_Y) > offset_y && offset_y > (-32.0f * (float)HUD_Y);
					if (active)
					{
						dedsec_last_hack = hack;
						dedsec_deselect_delay_start = get_current_time_millis();
					}
					else if (dedsec_last_hack && hack == dedsec_last_hack && GET_MILLIS_SINCE(dedsec_deselect_delay_start) < dedsec_deselect_delay)
					{
						active = true;
					}

					if (just_released)
					{
						if (active)
						{
							any_active = true;
							if (g_gui.isSoundEnabled())
							{
								AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
							}
							hack->execute(dedsec_selected_target);
							dedsec_last_hack = nullptr;
							dedsec_deselect_delay_start = 0;
						}
					}
					else
					{
						auto pos = g_renderer.posH2C((0.5f - offset_x) / HUD_X, (0.5f - offset_y) / HUD_Y);
						auto text_pos = g_renderer.posH2C((0.5f - offset_x) / HUD_X, (0.5f - offset_y) / HUD_Y - 12.0f);
						auto size = g_renderer.sizeH2C(64.0f, 64.0f);
						GRAPHICS::DRAW_RECT(pos.x / g_renderer.client_size.x, pos.y / g_renderer.client_size.y, size.x / g_renderer.client_size.x, size.y / g_renderer.client_size.y, active ? dedsec_r : 0, active ? dedsec_g : 0, active ? dedsec_b : 0, active ? 255 : 127, true);
						HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("CELL_EMAIL_BCON");
						HUD::SET_TEXT_COLOUR(255, 255, 255, 255);
						HUD::SET_TEXT_FONT(0);
						HUD::SET_TEXT_SCALE(0, 12.0f * (float)TEXT_HEIGHT_PX);
						HUD::SET_TEXT_JUSTIFICATION(0);
						//HUD::SET_TEXT_WRAP(0.5f - offset_x - (32.0f * (float)HUD_X), 0.5f - offset_x + (32.0f * (float)HUD_X));
						HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(Lang::get(hack->name).c_str());
						HUD::END_TEXT_COMMAND_DISPLAY_TEXT(text_pos.x / g_renderer.client_size.x, text_pos.y / g_renderer.client_size.y, 0);
					}
				}
				{
					auto name = dedsec_selected_target.getName();
					if (!name.empty())
					{
						const auto name_offset = hacks.size() >= 9 ? 0.475f : 0.5f;
						auto pos = g_renderer.posH2C((0.5f + cam_offset_x) / HUD_X, (name_offset + cam_offset_y) / HUD_Y - 164.0f);
						HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("CELL_EMAIL_BCON");
						HUD::SET_TEXT_COLOUR(dedsec_r, dedsec_g, dedsec_b, dedsec_a);
						HUD::SET_TEXT_FONT(0);
						HUD::SET_TEXT_SCALE(0, 12.0f * (float)TEXT_HEIGHT_PX);
						HUD::SET_TEXT_JUSTIFICATION(0);
						HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(name.c_str());
						HUD::END_TEXT_COMMAND_DISPLAY_TEXT(pos.x / g_renderer.client_size.x, pos.y / g_renderer.client_size.y, 0);
					}
				}
				if (just_released)
				{
					dedsec_selected_target.reset();
					if (!any_active && g_gui.isSoundEnabled())
					{
						AUDIO::PLAY_SOUND_FRONTEND(-1, "CANCEL", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
					}
				}
				else
				{
					unrestrictGameplayCam();
				}
			}
			else
			{
				if (!g_tb_screenshot_mode.isEnabled())
				{
					if (dedsec_passive_line)
					{
						DrawUtil3d::draw_focus_line(dedsec_target.getPos(), Renderer::convert_to_dx_colour(dedsec_r, dedsec_g, dedsec_b, dedsec_a));
					}
					if (dedsec_passive_box)
					{
						dedsec_target.getBox().drawLines(dedsec_r, dedsec_g, dedsec_b, dedsec_a);
					}
				}

				if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_PHONE))
				{
					if (g_gui.isSoundEnabled())
					{
						AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
					}
					if (dedsec_tutorial_state == DEDSEC_TUTORIAL_STATE_TARGET)
					{
						dedsec_tutorial_state++;
					}
					dedsec_selected_target = dedsec_target;
					/*if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_LOOK_BEHIND))
					{
						CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(CAM::GET_GAMEPLAY_CAM_COORD().look_at_yaw(dedsec_target_pos) - ENTITY::GET_ENTITY_ROTATION(g_player_ped, 2).z);
					}*/
					dedsec_selected_target_start_rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
				}
				else if (dedsec_tutorial_state == DEDSEC_TUTORIAL_STATE_TARGET)
				{
					showDedsecTutorialMessage(1);
				}
			}
		}
		dedsec_target.reset();
	}

	void AllEntitiesEveryTick::onTickEraseObjectsWithNoModelInfo()
	{
		__try
		{
			AbstractEntity::getAllObjects([](AbstractEntity&& ent)
			{
				auto k = ent.getModelHash();
				switch (k)
				{
				case 0x00000000:
				case 0x0E3A2DE0:
				case 0x84A79D0B:
				case 0xFA272290:
				case 0xFA272890:
					CONSUMER_CONTINUE;
				}
				auto mi = g_hooking.getModelInfo(k);
				if (mi == nullptr)
				{
					Util::toast(LANG_FMT("PTX_DELSTALE_T", joaatToString(k)), g_hooking.erase_objects_with_no_mi_notify.getToastFlags());
					if (!ent.removeFromPlaneOfExistenceForce())
					{
#ifdef STAND_DEBUG
						Util::toast("Failed to delete.");
#endif
					}
				}
				else if (ent.getPointer()->m_nFlags.bIsFrag)
				{
					if (pointers::rage_fwArchetype_GetFragType(mi) == nullptr)
					{
						Util::toast(LANG_FMT("PTX_DELSTALE_T_2", joaatToString(k)), g_hooking.erase_objects_with_no_mi_notify.getToastFlags());
						if (!ent.removeFromPlaneOfExistenceForce())
						{
#ifdef STAND_DEBUG
							Util::toast("Failed to delete.");
#endif
						}
					}
				}
				/*else if (ent.getPointer()->type == ENTITY_TYPE_LIGHT)
				{
					auto lightent = reinterpret_cast<CLightEntity*>(ent.getPointer());
					if (lightent->m_parentEntity.m_p == nullptr
						&& lightent->Get2dEffectType() == ET_LIGHT
						)
					{
						Util::toast("Light entity without parent, deleting.");
						ent.removeFromPlaneOfExistenceForce();
					}
				}*/
				CONSUMER_CONTINUE;
			});
		}
		__EXCEPTIONAL()
		{
		}
	}

	void AllEntitiesEveryTick::aimbotEnsureCamIsFree()
	{
		if (aimbot_captured_rot)
		{
			aimbot_captured_rot = false;
			Camgr::unlock_rot();
		}
	}

	void AllEntitiesEveryTick::aimbotDisable()
	{
		aimbotEnsureCamIsFree();
		aimbot_target.reset();
	}
}
