#include "CommandTabSelf.hpp"

#include "Camgr.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandLambdaListSelect.hpp"
#include "CommandLambdaSlider.hpp"
#include "CommandLambdaSliderScript.hpp"
#include "CommandListCommandBoxPresets.hpp"
#include "CommandListSelectWeapons.hpp"
#include "CommandListSelectParticle.hpp"
#include "CommandListSelectPedModels.hpp"
#include "CommandListSelectPointer.hpp"
#include "CommandSaveBodyguardPreset.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderPointer.hpp"
#include "CommandTogglePointer.hpp"
#include "eExplosionTag.hpp"
#include "input.hpp"
#include "PedModel.hpp"
#include "ProjectileMgr.hpp"
#include "Script.hpp"
#include "TickMgr.hpp"
#include "Util.hpp"

#include "CommandLevitate.hpp"
#include "CommandLevitationPresets.hpp"

#include "CommandFriction.hpp"
#include "CommandGracefulLanding.hpp"
#include "CommandSuperJump.hpp"
#include "CommandSuperFlight.hpp"
#include "CommandSuperRun.hpp"
#include "CommandMoveFreedom.hpp"
#include "CommandAirwalk.hpp"
#include "CommandAirSwim.hpp"
#include "CommandWaterWalk.hpp"
#include "CommandDrunkMode.hpp"
#include "CommandTennisMode.hpp"
#include "CommandFloppyMode.hpp"
#include "CommandReducedCollision.hpp"
#include "CommandNoCollision.hpp"
#include "CommandSelfFreeze.hpp"

#include "CommandListTransform.hpp"

#include "CommandWardrobe.hpp"
#include "CommandDisableOutfitRestrictions.hpp"
#include "CommandDisableInVehicleHairScale.hpp"
#include "CommandLockOutfit.hpp"
#include "outfit_data.hpp"
#include "CommandOutfitComponent.hpp"
#include "CommandOutfitComponentVariation.hpp"
#include "CommandHairHighlight.hpp"
#include "CommandSliderRainbow.hpp"
#include "CommandOutfitProp.hpp"
#include "CommandOutfitPropVariation.hpp"

#include "CommandListAnimations.hpp"
#include "CommandAutoCancelAnim.hpp"
#include "CommandScenarios.hpp"

#include "CommandPtfxTrail.hpp"
#include "CommandPtfxTrailParticle.hpp"

#include "CommandWalkStyle.hpp"
#include "CommandPreserveWalkStyleDuringCombat.hpp"
#include "CommandScaleCam.hpp"
#include "CommandScaleSpeed.hpp"
#include "CommandVisualZCorrection.hpp"
#include "CommandNoBlood.hpp"
#include "CommandMoist.hpp"
#include "CommandMoistLock.hpp"
#include "CommandInvisibility.hpp"
#include "CommandDeathParticle.hpp"
#include "CommandRespawnParticle.hpp"

#include "CommandGetGuns.hpp"
#include "CommandRemoveGuns.hpp"
#include "CommandPunishment.hpp"
#include "GunPunishments.hpp"
#include "CommandGravityGun.hpp"
#include "CommandVehicleGun.hpp"
#include "CommandListWeaponComponents.hpp"
#include "CommandListLockWeapons.hpp"
#include "CommandWeaponTint.hpp"
#include "CommandAimbot.hpp"
#include "CommandListSelectBonePointer.hpp"
#include "CommandTriggerBot.hpp"
#include "CommandProjectileMgr.hpp"
#include "CommandFillAmmo.hpp"
#include "CommandRemoveAmmo.hpp"
#include "CommandInfiniteAmmo.hpp"
#include "CommandNoReload.hpp"
#include "CommandNoSpread.hpp"
#include "CommandNoRecoil.hpp"
#include "CommandNoSpooling.hpp"
#include "CommandRapidFire.hpp"
#include "CommandRapidFireVeh.hpp"
#include "CommandDamage.hpp"
#include "CommandWeaponRange.hpp"
#include "CommandRocketSpeed.hpp"
#include "CommandSuperMelee.hpp"
#include "CommandShotgunBatchSize.hpp"
#include "CommandInstantLockon.hpp"
#include "CommandImpactParticles.hpp"
#include "CommandGunFreedom.hpp"
#include "CommandPedMoreStickies.hpp"
#include "CommandPedMoreFlares.hpp"
#include "CommandTpGun.hpp"
#include "CommandLaserSights.hpp"

#include "BodyguardMgr.hpp"

#include "CommandGod.hpp"
#include "CommandAutoHeal.hpp"
#include "CommandMaxHealth.hpp"
#include "CommandGrace.hpp"
#include "CommandSeatglue.hpp"
#include "CommandWanted.hpp"
#include "CommandWantedLock.hpp"
#include "CommandFakeWanted.hpp"
#include "CommandInfiniteStamina.hpp"
#include "CommandParalock.hpp"
#include "CommandRespawnRecall.hpp"
#include "CommandRespawnDelay.hpp"
#include "CommandRegenerationRate.hpp"
#include "CommandClumsiness.hpp"
#include "CommandNoRollCooldown.hpp"
#include "CommandRefillHealth.hpp"
#include "CommandRefillArmour.hpp"

namespace Stand
{
	CommandTabSelf::CommandTabSelf(CommandList* const parent)
		: CommandTab(parent, TAB_SELF, LOC("SLF"), { CMDNAME("tself"), CMDNAME("self") })
	{
	}

	void CommandTabSelf::populate()
	{
		// Movement
		{
			auto movement = this->createChild<CommandList>(LOC("MVMNT"));
			{
				// Levitation
				{
					auto* const levitate = movement->createChild<CommandList>(LOC("LEV"));

					auto speed = levitate->makeChild<CommandSliderFloat>(LOC("MOVSPD"), { CMDNAME("levitatespeed") }, NOLABEL, 1, 1000000, 100, 10);
					auto sprint_speed = levitate->makeChild<CommandSliderFloat>(LOC("SPRNTMUL"), { CMDNAME("levitatesprintmultiplier") }, LIT(LANG_FMT("SPRNTMUL_H", Input::vk_to_string(VK_SPACE))), 1, 1000000, 500, 10);
					auto acceleration = levitate->makeChild<CommandSliderFloat>(LOC("ACCEL"), CMDNAMES("levitateaccel"), LOC("ACCEL_H"), 0, 1000, 0, 25);
					auto ignore_pitch = levitate->makeChild<CommandToggle>(LOC("FC_NZ"), { CMDNAME("levitateignorepitch") }, LOC("FC_NZ_H"), true);
					auto keep_momentum = levitate->makeChild<CommandToggle>(LOC("KPMOMNTM"), { CMDNAME("keepmomentum") });
					auto apply_pitch_to_entity = levitate->makeChild<CommandToggle>(LOC("LEV_AP"), { CMDNAME("levitateapplypitch") });
					auto only_rotate_on_movement = levitate->makeChild<CommandToggle>(LOC("OROM"), CMDNAMES("levitaterotate"), NOLABEL, true);
					auto show_button_instructions = levitate->makeChild<CommandToggle>(LOC("SHWBI"), { CMDNAME("levitatebuttoninstructions") }, NOLABEL, true);
					auto passive_min = levitate->makeChild<CommandSliderFloat>(LOC("PASSMOV_MIN"), { CMDNAME("levitatepassivemin") }, NOLABEL, -1000000, 1000000, 0, 10);
					auto passive_max = levitate->makeChild<CommandSliderFloat>(LOC("PASSMOV_MAX"), { CMDNAME("levitatepassivemax") }, NOLABEL, -1000000, 1000000, 60, 10);
					auto passive_speed = levitate->makeChild<CommandSlider>(LOC("SPD"), { CMDNAME("levitatepassivespeed") }, NOLABEL, 0, 1000000, 5, 1);
					auto assist_up = levitate->makeChild<CommandSliderFloat>(LOC("FRC_U"), { CMDNAME("levitateassistup") }, NOLABEL, 0, 1000000, 60, 10);
					auto assist_down = levitate->makeChild<CommandSliderFloat>(LOC("FRC_D"), { CMDNAME("levitateassistdown") }, NOLABEL, 0, 1000000, 60, 10);
					auto assist_deadzone = levitate->makeChild<CommandSlider>(LOC("ASS_D"), { CMDNAME("levitateassistdeadzone") }, LOC("ASS_D_H"), 0, 100000, 13, 1);
					auto assist_snap = levitate->makeChild<CommandSliderFloat>(LOC("SNAP"), { CMDNAME("levitateassistsnap") }, NOLABEL, 0, 1000000, 10, 10);
					levitate->createChild<CommandLevitate>(speed.get(), sprint_speed.get(), acceleration.get(), ignore_pitch.get(), keep_momentum.get(), apply_pitch_to_entity.get(), only_rotate_on_movement.get(), show_button_instructions.get(), passive_min.get(), passive_max.get(), passive_speed.get(), assist_up.get(), assist_down.get(), assist_deadzone.get(), assist_snap.get());
					levitate->createChild<CommandLevitationPresets>();
					levitate->children.emplace_back(std::move(speed));
					levitate->children.emplace_back(std::move(sprint_speed));
					levitate->children.emplace_back(std::move(acceleration));
					levitate->children.emplace_back(std::move(ignore_pitch));
					levitate->children.emplace_back(std::move(keep_momentum));
					levitate->children.emplace_back(std::move(apply_pitch_to_entity));
					levitate->children.emplace_back(std::move(only_rotate_on_movement));
					levitate->children.emplace_back(std::move(show_button_instructions));
					levitate->children.emplace_back(std::make_unique<CommandDivider>(levitate, LOC("PASSMOV")));
					levitate->children.emplace_back(std::move(passive_min));
					levitate->children.emplace_back(std::move(passive_max));
					levitate->children.emplace_back(std::move(passive_speed));
					levitate->children.emplace_back(std::make_unique<CommandDivider>(levitate, LOC("ASS")));
					levitate->children.emplace_back(std::move(assist_up));
					levitate->children.emplace_back(std::move(assist_down));
					levitate->children.emplace_back(std::move(assist_deadzone));
					levitate->children.emplace_back(std::move(assist_snap));
				}

				movement->children.emplace_back(std::make_unique<CommandLambdaSlider<CommandSliderFloat>>(movement, LOC("WLKSPD"), CMDNAMES("walkspeed"), NOLABEL, -1000000, 1000000, 100, 10, [](int value, Click&)
				{
					g_gui.user_defined_run_speed = (float)value / 100.0f;
				}));
				movement->createChild<CommandFriction>();
				movement->createChild<CommandGracefulLanding>();
				movement->createChild<CommandSuperJump>();
				{
					auto super_flight = movement->createChild<CommandList>(LOC("FLY"));

					auto fly = super_flight->createChild<CommandSuperFlight>();
					super_flight->createChild<CommandTogglePointer>(&fly->bi_jump2fly, LOC("SHWBI_JMP2FLY"));
					super_flight->createChild<CommandTogglePointer>(&fly->bi_controls, LOC("SHWBI_CTRLS"));
				}
				movement->createChild<CommandWaterWalk>();
				movement->children.emplace_back(std::make_unique<CommandLambdaSlider<CommandSliderFloat>>(movement, LOC("SWMSPD"), CMDNAMES("swimspeed"), NOLABEL, -1000000, 1000000, 100, 10, [](int value, Click&)
				{
					g_gui.user_defined_swim_speed = (float)value / 100.0f;
				}));
				movement->createChild<CommandSuperRun>();
				movement->createChild<CommandMoveFreedom>();
				movement->createChild<CommandAirwalk>();
				movement->createChild<CommandAirSwim>();
				movement->createChild<CommandDrunkMode>();
				movement->createChild<CommandTennisMode>();

				// Floppy Mode
				{
					auto list = movement->createChild<CommandList>(LOC("FLOPPY"));
					auto flop = list->createChild<CommandFloppyMode>();
					flop->movement_speed = list->createChild<CommandSliderFloat>(LOC("MOVSPD"), CMDNAMES_OBF("floppyspeed"), NOLABEL, 1, 1000000, 100, 10);
					flop->sprint_speed = list->createChild<CommandSliderFloat>(LOC("SPRNTMUL"), CMDNAMES_OBF("floppysprint"), LIT(LANG_FMT("SPRNTMUL_H", Input::vk_to_string(VK_SPACE))), 1, 500, 500, 10);
					flop->button_instructions = list->createChild<CommandToggle>(LOC("SHWBI"), CMDNAMES_OBF("floppyinstructions"), NOLABEL, true);
					flop->prioritise_synching = list->createChild<CommandToggle>(LOC("FLOPPY_SYNCH"), CMDNAMES_OBF("floppysynch"), LOC("FLOPPY_SYNCH_H"));
				}

				movement->createChild<CommandGhost>();
				movement->createChild<CommandNoCollision>();
				movement->createChild<CommandSelfFreeze>();
			}
		}

		// Appearance
		{
			auto appearance = this->createChild<CommandList>(LOC("VIS"));

			appearance->children.emplace_back(std::make_unique<CommandListTransform>(appearance));

			// Outfit
			{
				auto outfit = appearance->createChild<CommandList>(LOC("OTFT"), { CMDNAME("outfit") });

				outfit->createChildWithBuilder<CommandWardrobe>();
				outfit->createChild<CommandDisableOutfitRestrictions>();
				outfit->createChild<CommandDisableInVehicleHairScale>();
				outfit->createChild<CommandLockOutfit>();
				for (const PedComponent& component : ped_components)
				{
					auto component_command = outfit->createChild<CommandOutfitComponent>(&component);
					auto component_variation_command = outfit->createChild<CommandOutfitComponentVariation>(&component);
					CommandLockOutfit::ped_component_commands[component.id] = { component_command, component_variation_command };
					if (component.id == PV_COMP_HAIR) // Hair
					{
						CommandLockOutfit::hair_highlight = outfit->createChild<CommandHairHighlight>();
						outfit->createChild<CommandSliderRainbow>(component_variation_command, LOC("HAIRR"), { CMDNAME("hairrainbow"), CMDNAME("hairainbow") });
					}
				}
				for (const PedProp& prop : ped_props)
				{
					auto prop_command = outfit->createChild<CommandOutfitProp>(&prop);
					auto prop_variation_command = outfit->createChild<CommandOutfitPropVariation>(&prop);
					CommandLockOutfit::ped_prop_commands[prop.id] = { prop_command, prop_variation_command };
				}
			}

			// Animations
			{
				CommandIssuable::collapse_command_names.emplace(L"scen");
				CommandIssuable::collapse_command_names.emplace(L"anim");

				auto animations = appearance->createChild<CommandList>(LOC("ANIM"));

				animations->createChild<CommandLambdaActionScript>(LOC("CANCL"), { CMDNAME("cancelanim") }, NOLABEL, [](const Click& click)
				{
					Animations::reset();
				});

				animations->createChild<CommandScenarios>();
				animations->createChild<CommandListAnimations>();

				// Object Animations
				{
					auto object = animations->createChild<CommandList>(LOC("OBJANIM"), CMDNAMES_OBF("objectanimations", "propanimations"));

					for (auto& anim : Animations::object_animations)
					{
						object->createChild<CommandAnimation<ComplexObjectAnimation>>(anim);
					}
				}

				animations->createChild<CommandDivider>(LOC("STTNGS"));
				animations->createChild<CommandTogglePointer>(&CommandListAnimations::partial_movement, LOC("PART_MVMT"), {}, LOC("PART_MVMT_H"));
				animations->createChild<CommandTogglePointer>(&CommandListAnimations::looped, LOC("LOOPED"), {}, LOC("LOOPED_H"));
				animations->createChild<CommandAutoCancelAnim>();
			}

			// PTFX Trails
			{
				auto list = appearance->createChild<CommandList>(LOC("PTFXTRAIL"));
				auto particle = list->createChild<CommandPtfxTrailParticle>();
				particle->interval = list->createChild<CommandSlider>(LOC("INTVAL_MS"), CMDNAMES_OBF("trailinterval"), NOLABEL, 0, 1000, 0);
				particle->scale = list->createChild<CommandSliderFloat>(LOC("SCLE"), CMDNAMES_OBF("trailscale"), NOLABEL, 10, 1000, 15);
				particle->offset = list->createChild<CommandSliderFloat>(LOC("HOROFF"), CMDNAMES_OBF("trailoffset"), NOLABEL, -1000, 1000, 0);
				list->createChild<CommandDivider>(LOC("BONES"));
				for (auto group : major_bone_labels)
				{
					auto label = group.second;
					bool default_on = false;

					switch (group.first)
					{
					case SKEL_R_Hand:
					case SKEL_L_Hand:
					case SKEL_R_Foot:
					case SKEL_L_Foot:
						default_on = true;
						break;
					}

					list->createChild<CommandPtfxTrail>(group.first, std::move(label), {}, default_on);
				}
			}

			appearance->createChild<CommandWalkStyle>();
			appearance->createChild<CommandPreserveWalkStyleDuringCombat>();
			appearance->createChild<CommandLambdaSliderScript<CommandSliderFloat>>(LOC("SCLE"), { CMDNAME("scale") }, LOC("SCLE_H"), -1000000, 1000000, 100, 10, [](int value, const Click&)
			{
				g_gui.character_scale_multiplier = (float)value / 100.0f;
				PED::SET_PED_LOD_MULTIPLIER(g_player_ped, abs(g_gui.character_scale_multiplier));
				if (g_gui.character_scale_multiplier == 1.0f)
				{
					Camgr::unlock_pos();
				}
				if (g_gui.character_scale_affects_camera_command->m_on)
				{
					g_gui.set_camera_distance_from_character_multiplier_all_things_considered();
				}
			});
			g_gui.character_scale_affects_camera_command = appearance->createChild<CommandScaleCam>();
			g_gui.character_scale_affects_speed_command = appearance->createChild<CommandScaleSpeed>();
			appearance->createChild<CommandVisualZCorrection>();
			appearance->createChild<CommandNoBlood>();
			auto moist = appearance->createChild<CommandMoist>();
			appearance->createChild<CommandMoistLock>(std::move(moist));
			appearance->createChild<CommandInvisibility>();
			appearance->createChild<CommandDeathParticle>();
			appearance->createChild<CommandRespawnParticle>();
		}

		// Weapons
		{
			auto weapons = this->createChild<CommandList>(LOC("WPN"));

			weapons->createChildWithBuilder<CommandGetGuns>();
			weapons->createChildWithBuilder<CommandRemoveGuns>();

			{
				auto shoot_punishments = weapons->createChild<CommandList>(LOC("GSHTPNSH"));
				for (const Punishment* const p : Punishments::all)
				{
					if (p->isApplicable(PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NOTBROAD | PUNISHMENTFOR_USERACTION))
					{
						shoot_punishments->createChild<CommandPunishment>(&GunPunishments::shoot_punishments, *p, std::vector<CommandName>(p->gunshoot_command_names));
					}
				}
			}
			{
				auto aim_punishments = weapons->createChild<CommandList>(LOC("GAIMPNSH"));
				for (const Punishment* const p : Punishments::all)
				{
					if (p->isApplicable(PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NOTBROAD | PUNISHMENTFOR_USERACTION))
					{
						aim_punishments->createChild<CommandPunishment>(&GunPunishments::aim_punishments, *p);
					}
				}
			}

			{
				auto* gravgun = weapons->createChild<CommandList>(LOC("GRTYGUN"));

				auto contrived_physics = gravgun->makeChild<CommandToggle>(LOC("CTRVPHYS"), { CMDNAME("gravityguncontrivedphysics") });
				auto force_multiplier = gravgun->makeChild<CommandSliderFloat>(LOC("FRCMUL"), { CMDNAME("gravitygunforce") }, NOLABEL, 1, 1000, 100);
				auto collision = gravgun->makeChild<CommandToggle>(LOC("COL"), { CMDNAME("gravityguncollision") }, NOLABEL, true);
				gravgun->createChild<CommandGravityGun>(contrived_physics.get(), force_multiplier.get(), collision.get());
				gravgun->children.emplace_back(std::move(contrived_physics));
				gravgun->children.emplace_back(std::move(force_multiplier));
				gravgun->children.emplace_back(std::move(collision));
			}

			weapons->createChild<CommandVehicleGun>();
			weapons->createChild<CommandListWeaponComponents>();
			weapons->createChild<CommandListLockWeapons>();

			{
				auto tint = weapons->createChild<CommandWeaponTint>();
				weapons->children.emplace_back(std::make_unique<CommandSliderRainbow>(weapons, tint, LOC("GUNTINTR"), CMDNAMES("weapontintrainbow")));
			}

			// Aimbot
			{
				auto aimbot = weapons->createChild<CommandList>(LOC("AIMBT"));

				aimbot->createChild<CommandAimbot>();

				// Target Selection
				{
					auto target_selection = aimbot->createChild<CommandList>(LOC("TRGSEL"));
					AllEntitiesEveryTick::aimbot_targeting.populateOptions(target_selection, CMDNAMES_OBF("aimbot"), false, true);
				}

				aimbot->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::aimbot_target_box, LOC("DRWBOX"), { CMDNAME("aimbotbox") });
				aimbot->createChild<CommandSliderPointer<CommandSlider>>(LOC("AIMBT_LCKOUT"), { CMDNAME("aimbotlockout") }, LOC("AIMBT_LCKOUT_H"), & AllEntitiesEveryTick::aimbot_lockout, 0, 10000, 50);
				aimbot->createChild<CommandSliderPointer<CommandSlider>>(LOC("SMTHNG"), { CMDNAME("aimbotsmoothing") }, NOLABEL, &AllEntitiesEveryTick::aimbot_smoothing, 0, 1000);
				aimbot->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::aimbot_los_check, LOC("LOSCHECK"), { CMDNAME("aimbotloscheck") });
				aimbot->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::aimbot_magic_bullets, LOC("MAGICB"), CMDNAMES_OBF("aimbotmagicbullets"));
				aimbot->createChild<CommandListSelectBonePointer>(LOC("TGTBONE"), CMDNAMES_OBF("aimbotbone"), NOLABEL, &AllEntitiesEveryTick::aimbot_ped_bone);
			}

			// Triggerbot
			{
				auto triggerbot_list = weapons->createChild<CommandList>(LOC("TRIGBT"));

				auto triggerbot = triggerbot_list->createChild<CommandTriggerBot>();
				triggerbot_list->createChild<CommandTogglePointer>(&triggerbot->magic_bullets, LOC("MAGICB"), CMDNAMES_OBF("magicbullets"), LOC("MAGICB_H"));
				triggerbot_list->createChild<CommandDivider>(LOC("TRGSEL"));
				triggerbot->targeting.populateOptions(triggerbot_list, CMDNAMES_OBF("triggerbot"), false, true);
			}

			// Proximity Rockets
			{
				auto prox = weapons->createChild<CommandList>(LOC("PROXYROCKET"));
				ProjectileMgr::proximity_rockets = prox->createChild<CommandProjectileMgr>(LOC("PROXYROCKET"), CMDNAMES_OBF("proxyrockets"), LOC("PROXYROCKET_H"));
				ProjectileMgr::proximity_rocket_fuse = prox->createChild<CommandSliderFloat>(LOC("RNGE"), CMDNAMES_OBF("proxyrange"), NOLABEL, 50, 500, 300, 25);
			}

			weapons->createChild<CommandFillAmmo>();
			weapons->createChild<CommandRemoveAmmo>();
			weapons->createChild<CommandInfiniteAmmo>();
			weapons->createChild<CommandNoReload>();
			weapons->createChild<CommandNoSpread>();
			weapons->createChild<CommandNoRecoil>();
			weapons->createChild<CommandNoSpooling>();
			weapons->createChild<CommandRapidFire>();
			weapons->createChild<CommandRapidFireVeh>();
			weapons->createChild<CommandDamage>();
			weapons->createChild<CommandWeaponRange>();
			weapons->createChild<CommandRocketSpeed>();
			weapons->createChild<CommandSuperMelee>();
			weapons->createChild<CommandShotgunBatchSize>();
			weapons->createChild<CommandInstantLockon>();
			ProjectileMgr::proximity_mines = weapons->createChild<CommandProjectileMgr>(LOC("INSTAPROXY"), CMDNAMES_OBF("instantproxy"));
			weapons->createChild<CommandTogglePointer>(&TickMgr::expammo.every_tick, LOC("EXPAMMO"), { CMDNAME("explosivehits"), CMDNAME("expammo") }, NOLABEL, CMDFLAGS_TOGGLE | CMDFLAG_TUTORIAL_CTX_POINT);
			weapons->createChild<CommandLambdaListSelect>(LOC("EXPTYP"), { CMDNAME("exptype") }, LOC("EXPTYP_H"), [](long long value, Click&)
			{
				TickMgr::expammo_type = (int)value;
			}, eExplosionTag_getListActionItems(), EXP_TAG_STICKYBOMB, CMDFLAGS_LIST_SELECT, CMDFLAG_FEATURELIST_FINISHLIST);

			// Impact Particles
			{
				auto imp = weapons->createChild<CommandImpactParticles>();
				imp->particle = weapons->createChild<CommandListSelectParticle>(LOC("PARTICLE"), LOC("IMPPART_H"));
			}

			weapons->createChild<CommandGunFreedom>();
			weapons->createChild<CommandPedMoreStickies>();
			weapons->createChild<CommandPedMoreFlares>();
			weapons->createChild<CommandTpGun>();
			weapons->createChild<CommandLaserSights>();
		}

		// Bodyguards
		{
			BodyguardMgr::list = this->createChild<CommandList>(LOC("BDYGUARD"));

			// Spawn
			{
				auto spawn = BodyguardMgr::list->createChild<CommandList>(LOC("SPWN"));

				spawn->createChild<CommandLambdaActionScript>(LOC("SPWN"), { CMDNAME("spawnbodyguards"), CMDNAME("spawnguards") }, NOLABEL, [](const Click& click)
				{
					BodyguardMgr::push();
				});

				BodyguardMgr::count = spawn->createChild<CommandSlider>(LOC("COUNT"), { CMDNAME("bodyguardcount"), CMDNAME("guardcount") }, NOLABEL, 1, 7, 1, 1);
				BodyguardMgr::model = spawn->createChild<CommandListSelectPedModels>({ CMDNAME("bodyguardmodel"), CMDNAME("guardmodel") }, NOLABEL, ATSTRINGHASH("player_two"));
				BodyguardMgr::primary_weapon = spawn->createChild<CommandListSelectWeapons>(LOC("PRIM_WEAP"), CMDNAMES_OBF("bodyguardprimary", "guardprimary"), LOC("WEAP_H"), ATSTRINGHASH("WEAPON_PISTOL"));
				BodyguardMgr::secondary_weapon = spawn->createChild<CommandListSelectWeapons>(LOC("SCND_WEAP"), CMDNAMES_OBF("bodyguardsecondary", "guardsecondary"), LOC("WEAP_H"), ATSTRINGHASH("WEAPON_PISTOL"));
			}

			BodyguardMgr::list->createChild<CommandListCommandBoxPresets<CommandSaveBodyguardPreset>>(CMDNAMES_OBF("bodyguardpresets"), "Bodyguards");

			// Behavior
			{
				auto behavior = BodyguardMgr::list->createChild<CommandList>(LOC("BEHVR"));

				BodyguardMgr::formation = behavior->createChild<CommandLambdaListSelect>(LOC("FORMATION"), CMDNAMES_OBF("bodyguardformation"), NOLABEL, [](long long value, Click& click)
				{
					click.ensureScriptThread([value]
					{
						PED::SET_GROUP_FORMATION(BodyguardMgr::getGroup(), (int)value);
					});
				}, std::vector<CommandListActionItemData>{
					{ 0, LOC("NONE"), CMDNAMES_OBF("none") },
					{ 3, LOC("LINE"), CMDNAMES_OBF("line") },
					{ 2, LOC("CIRCLE"), CMDNAMES_OBF("circle") },
					{ 1, LOC("ALT_CIRCLE"), CMDNAMES_OBF("altcircle", "alternatecircle") }
				}, 0);

				BodyguardMgr::relationship = behavior->createChild<CommandListSelect>(LOC("AGGR"), CMDNAMES_OBF("bodyguardaggression"), NOLABEL, std::vector<CommandListActionItemData>{
					{ ATSTRINGHASH("rgFM_AiLike_HateAiHate"), LOC("AGGR1"), CMDNAMES_OBF("friendly"), LOC("AGGR1_H") },
					{ ATSTRINGHASH("rgFM_HateEveryOne"), LOC("AGGR2"), CMDNAMES_OBF("defensive"), LOC("AGGR2_H")},
					{ ATSTRINGHASH("PLAYER"), LOC("SPFDHTPRST_2"), CMDNAMES_OBF("aggressive"), LOC("AGGR3_H")}
				}, ATSTRINGHASH("rgFM_HateEveryOne"));

				behavior->createChild<CommandDivider>(LOC("XCLDS"));

				BodyguardMgr::ignore_players = behavior->createChild<CommandToggle>(LOC("XCLDPR"), CMDNAMES_OBF("bodyguardsignoreplayers"), LOC("XCLD_GRD"));
				BodyguardMgr::ignore_friends = behavior->createChild<CommandToggle>(LOC("XCLDF"), CMDNAMES_OBF("bodyguardsignorefriends"), LOC("XCLD_GRD"));
				BodyguardMgr::ignore_authorities = behavior->createChild<CommandToggle>(LOC("XCLD50"), CMDNAMES_OBF("bodyguardsignoreauthorities"), LOC("XCLD_GRD"));
				BodyguardMgr::ignore_crew_members = behavior->createChild<CommandToggle>(LOC("XCLDCM"), CMDNAMES_OBF("bodyguardsignorecrewmembers"), LOC("XCLD_GRD"));
				BodyguardMgr::ignore_organisation_members = behavior->createChild<CommandToggle>(LOC("XCLDOG"), CMDNAMES_OBF("bodyguardsignoreorganisationmembers"), LOC("XCLD_GRD"));
			}

			BodyguardMgr::immortal = BodyguardMgr::list->createChild<CommandToggle>(LOC("GOD"), CMDNAMES_OBF("bodyguardimmortality", "bodyguardgodmode"));

			BodyguardMgr::list->createChild<CommandLambdaActionScript>(LOC("PLYSMMN"), { CMDNAME("tpbodyguards"), CMDNAME("tpguards") }, NOLABEL, [](const Click& click)
			{
				BodyguardMgr::summon();
			});

			BodyguardMgr::teleport_when_far = BodyguardMgr::list->createChild<CommandToggle>(LOC("BDYGUARD_TP"), CMDNAMES_OBF("bodyguardsteleportwhenfar"));

			BodyguardMgr::list->createChild<CommandLambdaActionScript>(LOC("BDYGUARD_DEL"), { CMDNAME("delbodyguards"), CMDNAME("delguards") }, NOLABEL, [](const Click& click)
			{
				BodyguardMgr::clear();
			});
		}

		auto god = this->createChild<CommandGod>();
		auto autoheal = this->createChild<CommandAutoHeal>();
		this->createChild<CommandMaxHealth>();
		auto grace = this->createChild<CommandGrace>();
		this->createChild<CommandSeatglue>();
		auto wanted_slider = this->createChild<CommandWanted>();
		this->createChild<CommandWantedLock>(std::move(wanted_slider));
		this->createChild<CommandFakeWanted>();
		this->createChild<CommandInfiniteStamina>();
		this->createChild<CommandParalock>();
		this->createChild<CommandClumsiness>(grace);
		this->createChild<CommandNoRollCooldown>();
		this->createChild<CommandRespawnRecall>();
		this->createChild<CommandRespawnDelay>();
		this->createChild<CommandRegenerationRate>();
		this->createChild<CommandRefillHealth>();
		this->createChild<CommandRefillArmour>();
		this->createChild<CommandLambdaAction>(LOC("EWO"), CMDNAMES("ewo", "suicide", "kms", "kys"), NOLABEL, [=](Click& click)
		{
			click.ensureYieldableScriptThread([god, autoheal]
			{
				const bool was_god = god->m_on;
				const bool was_autoheal = autoheal->m_on;
				if (was_god)
				{
					Click click(CLICK_AUTO, TC_SCRIPT_YIELDABLE);
					god->onClick(click);
				}
				if (was_autoheal)
				{
					Click click(CLICK_AUTO, TC_SCRIPT_YIELDABLE);
					autoheal->onClick(click);
				}
				ENTITY::SET_ENTITY_HEALTH(g_player_ped, 0, 0, 0);
				Script::current()->yield(1000);
				if (was_god)
				{
					Click click(CLICK_AUTO, TC_SCRIPT_YIELDABLE);
					god->onClick(click);
				}
				if (was_autoheal)
				{
					Click click(CLICK_AUTO, TC_SCRIPT_YIELDABLE);
					autoheal->onClick(click);
				}
			});
		});
	}
}
