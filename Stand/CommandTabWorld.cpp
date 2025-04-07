#include "CommandTabWorld.hpp"

#include "CommandColour.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandSliderFloat.hpp"
#include "CommandSliderPointer.hpp"
#include "CommandTogglePointer.hpp"
#include "create_rainbow.hpp"

#include "CommandTeleport.hpp"
#include "CommandRepeatTeleport.hpp"
#include "CommandUndoTp.hpp"
#include "CommandWaypoint.hpp"

#include "CommandMyPosition.hpp"
#include "CommandSavePosMe.hpp"
#include "CommandSavePosWp.hpp"

#include "CommandClearArea.hpp"

#include "CommandTrafficColour.hpp"
#include "CommandTrafficDisable.hpp"
#include "CommandTrafficLod.hpp"
#include "CommandTrafficModelFlag.hpp"

#include "CommandDisablePeds.hpp"

#include "CommandWorldEditor.hpp"

#include "CommandWaterWaveBehaviour.hpp"
#include "CommandWaterHeight.hpp"
#include "CommandWaterStrengthOverride.hpp"
#include "CommandWaterOpacityDifferential.hpp"

#include "CommandWorldState.hpp"

#include "punishments.hpp"
#include "CommandExistencePunishment.hpp"
#include "CommandNpcProximityPunishment.hpp"
#include "CommandPunishableProximity.hpp"
#include "CommandNpcHostilityPunishment.hpp"
#include "CommandNpcAimPunishment.hpp"
#include "CommandNpcNeedsToAimAtUser.hpp"
#include "CommandPlayerAimPunishment.hpp"
#include "CommandPlayerNeedsToAimAtUser.hpp"

#include "CommandNpcBoneEsp.hpp"

#include "CommandEspTags.hpp"
#include "CommandPlayerBoneEsp.hpp"
#include "CommandPlayerNameEsp.hpp"
#include "CommandPlayerBoxEsp.hpp"
#include "CommandPlayerLineEsp.hpp"

#include "CommandWorldDoors.hpp"
#include "CommandLambdaToggle.hpp"

#include "CommandDedsec.hpp"
#include "CommandDedsecColour.hpp"
#include "dedsec_hacks.hpp"

#include "CommandGeoGuessr.hpp"

#include "CommandOverrideWeather.hpp"

#include "CommandClock.hpp"

#include "CommandAestheticLight.hpp"
#include "CommandAestheticLightPlacement.hpp"
#include "CommandAestheticRainbow.hpp"

#include "CommandWorldBorder.hpp"
#include "CommandBlackout.hpp"
#include "CommandTrains.hpp"

#include "CommandListInteriors.hpp"

#include "ipls.hpp"
#include "CommandIpl.hpp"
#include "CommandNorthYanktonMap.hpp"

#include "CommandTeleportParticle.hpp"

#include "CommandDisableSkybox.hpp"
#include "CommandArWaypoint.hpp"
#include "CommandArGps.hpp"
#include "CommandClouds.hpp"

namespace Stand
{
	CommandTabWorld::CommandTabWorld(CommandList* const parent)
		: CommandTab(parent, TAB_WORLD, LOC("WRLD"), { CMDNAME("tworld"), CMDNAME("world") })
	{
	}

	void CommandTabWorld::populate()
	{
		{
			auto places = this->createChild<CommandList>(LOC("PLCS"));

			places->createChildWithBuilder<CommandTeleport>();
			places->createChild<CommandRepeatTeleport>();
			places->createChild<CommandUndoTp>();
			places->createChildWithBuilder<CommandWaypoint>();

			// Position
			{
				auto position = places->createChild<CommandMyPosition>();
				position->createChild<CommandSavePosMe>();
				position->createChild<CommandSavePosWp>();
				position->populate();
			}

			places->createChild<CommandListInteriors>();

			{
				auto ipls_list = places->createChild<CommandList>(LOC("IPLS"), { CMDNAME("ipls") }, LOC("IPLS_H"));

				auto tp_toggle = ipls_list->createChild<CommandToggle>(LOC("IPLTP"), { CMDNAME("ipltp") }, LOC("IPLTP_H"), true);
				for (const IplItem& ipl : ipls)
				{
					if (ipl.menu_name == ATSTRINGHASH("IPL_CLKNBLLFCTRY"))
					{
						ipls_list->createChild<CommandNorthYanktonMap>();
					}
					ipls_list->createChild<CommandIpl>(tp_toggle, &ipl, ipl.special_behaviour >= IPLSPEC_MP_WORLD_STATE ? LOC("IPL_MP") : NOLABEL);
				}
			}

			places->createChild<CommandTeleportParticle>();
		}

		// Inhabitants
		{
			auto inhabs = this->createChild<CommandList>(LOC("INHAB"));

			inhabs->createChild<CommandClearArea>();

			// Traffic
			{
				auto traffic = inhabs->createChild<CommandList>(LOC("TRFC"));

				traffic->createChild<CommandTrafficColour>();
				traffic->createChild<CommandTrafficDisable>();
				traffic->createChild<CommandTrafficLod>();
				traffic->createChild<CommandTrafficModelFlag>(CVehicleModelInfo::FLAG_HAS_GLIDER, LOC("GLDABL"), CMDNAMES("trafficglide"));
			}

			// Pedestrians
			{
				auto peds = inhabs->createChild<CommandList>(LOC("PEDS"));

				peds->createChild<CommandDisablePeds>();
			}

			// NPC Existence Punishments
			{
				auto existence_punishments = inhabs->createChild<CommandList>(LOC("XSTPNSH"), { CMDNAME("existencepunishments") });
				for (const Punishment* const p : Punishments::all)
				{
					if (p->isApplicable(PUNISHMENTFOR_NPC))
					{
						existence_punishments->createChild<CommandExistencePunishment>(*p);
					}
				}
			}

			// NPC Proximity Punishments
			{
				auto proximity_punishments = inhabs->createChild<CommandList>(LOC("NPRXPNSH"), { CMDNAME("proximitypunishments") });
				proximity_punishments->createChild<CommandPunishableProximity>();
				proximity_punishments->createChild<CommandDivider>(LOC("NPRXPNSH"));
				for (const Punishment* const p : Punishments::all)
				{
					if (p->isApplicable(PUNISHMENTFOR_NPC | PUNISHMENTFOR_NOTBROAD))
					{
						proximity_punishments->createChild<CommandNpcProximityPunishment>(*p);
					}
				}
			}

			// NPC Hostility Punishments
			{
				auto list = inhabs->createChild<CommandList>(LOC("NHOSPNSH"), CMDNAMES_OBF("hostilitypunishments"));


				list->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::npc_hostility_include_everyone, LOC("INCLUDE_EVERYONE"), {}, LOC("INCLUDE_H1"));
				list->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::npc_hostility_include_everyone_in_missions, LOC("INCLUDE_EVERYONE_M"), {}, LOC("INCLUDE_H2"));
				list->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::npc_hostility_include_friends, LOC("INCLUDE_FRIEND"), {}, LOC("INCLUDE_H1"));
				list->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::npc_hostility_include_passengers, LOC("INCLUDE_PASS"), {}, LOC("INCLUDE_H1"));
				list->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::npc_hostility_include_crew, LOC("INCLUDE_CREW"), {}, LOC("INCLUDE_H1"));
				list->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::npc_hostility_include_org, LOC("INCLUDE_ORG"), {}, LOC("INCLUDE_H1"));
				list->createChild<CommandDivider>(LOC("NHOSPNSH"));

				for (const Punishment* const p : Punishments::all)
				{
					if (p->isApplicable(PUNISHMENTFOR_NPCHOSTILITY | PUNISHMENTFOR_NOTBROAD | PUNISHMENTFOR_AIMINGAUSER))
					{
						list->createChild<CommandNpcHostilityPunishment>(*p);
					}
				}
			}

			// NPC Aim Punishments
			{
				auto npc_aim_punishments = inhabs->createChild<CommandList>(LOC("NAIMPNSH"), { CMDNAME("aimpunishments") }, LOC("NAIMPNSH_H"));
				npc_aim_punishments->createChild<CommandNpcNeedsToAimAtUser>();
				npc_aim_punishments->createChild<CommandDivider>(LOC("NAIMPNSH"));
				for (const Punishment* const p : Punishments::all)
				{
					if (p->isApplicable(PUNISHMENTFOR_NPC | PUNISHMENTFOR_AIMINGAUSER | PUNISHMENTFOR_NOTBROAD))
					{
						npc_aim_punishments->createChild<CommandNpcAimPunishment>(*p);
					}
				}
			}

			// Player Aim Punishments
			{
				auto player_aim_punishments = inhabs->createChild<CommandList>(LOC("PAIMPNSH"), { CMDNAME("playeraimpunishments") }, LOC("PAIMPNSH_H"));
				AllEntitiesEveryTick::player_aim_excludes.populateList(player_aim_punishments, {}, PlayerExcludes::SHOW_ORG_MEMBERS);
				player_aim_punishments->createChild<CommandPlayerNeedsToAimAtUser>();
				player_aim_punishments->createChild<CommandDivider>(LOC("PAIMPNSH"));
				for (const Punishment* const p : Punishments::all)
				{
					if (p->isApplicable(PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_AIMINGAUSER | PUNISHMENTFOR_NOTBROAD))
					{
						player_aim_punishments->createChild<CommandPlayerAimPunishment>(*p);
					}
				}
			}

			// NPC ESP
			{
				auto npc_esp = inhabs->createChild<CommandList>(LOC("NESP"));
				auto exclude_dead = npc_esp->makeChild<CommandToggle>(LOC("XCLDED"));
				auto default_colour = npc_esp->makeChild<CommandColour>(LOC("CLR"), CMDNAMES("npcespcolour"), NOLABEL, DirectX::SimpleMath::Color{ 1.0f, 0.0f, 1.0f, 0.78431f });
				create_rainbow(default_colour.get());
				npc_esp->createChild<CommandNpcBoneEsp>(exclude_dead.get(), default_colour.get());
				npc_esp->children.push_back(std::move(exclude_dead));
				npc_esp->children.push_back(std::move(default_colour));
			}

			// Player ESP
			{
				auto* const player_esp = inhabs->createChild<CommandList>(LOC("PESP"));
				auto default_colour = player_esp->makeChild<CommandColour>(LOC("DFLTCLR"), CMDNAMES("defaultespcolour"), NOLABEL, DirectX::SimpleMath::Color{ 1.0f, 0.0f, 1.0f, 0.78431f });
				auto tag_colours = player_esp->makeChild<CommandEspTags>(default_colour.get());
				create_rainbow(default_colour.get());

				player_esp->createChild<CommandPlayerBoneEsp>(default_colour.get(), tag_colours.get());

				// Name ESP
				{
					auto* name_esp = player_esp->createChild<CommandList>(LOC("NAMESP"));

					auto max_distance = name_esp->makeChild<CommandSlider>(LOC("MAXDIST"), { CMDNAME("esprange") }, NOLABEL, 1, 100000, 1000, 50);
					auto show_tags = name_esp->makeChild<CommandToggle>(LOC("SHWTGS"), { CMDNAME("esptags") }, NOLABEL, true);
					auto min_text_scale = name_esp->makeChild<CommandSlider>(LOC("MINTXTSCL"), { CMDNAME("esptextmin") }, NOLABEL, 1, 10000, 50);
					auto max_text_scale = name_esp->makeChild<CommandSlider>(LOC("MAXTXTSCL"), { CMDNAME("esptextmax") }, NOLABEL, 1, 10000, 100);
					auto invert_text_scale = name_esp->makeChild<CommandToggle>(LOC("INVTXTSCL"), { CMDNAME("esptextinverse") }, NOLABEL, true);
					name_esp->createChild<CommandPlayerNameEsp>(max_distance.get(), show_tags.get(), min_text_scale.get(), max_text_scale.get(), invert_text_scale.get(), default_colour.get(), tag_colours.get());
					name_esp->children.emplace_back(std::move(max_distance));
					name_esp->children.emplace_back(std::move(show_tags));
					name_esp->children.emplace_back(std::move(min_text_scale));
					name_esp->children.emplace_back(std::move(max_text_scale));
					name_esp->children.emplace_back(std::move(invert_text_scale));
				}

				// Box ESP
				{
					auto box_esp = player_esp->createChild<CommandList>(LOC("BOXESP"));
					auto max_distance = box_esp->makeChild<CommandSlider>(LOC("MAXDIST"), { CMDNAME("boxesprange") }, NOLABEL, 1, 100000, 1000, 50);
					box_esp->createChild<CommandPlayerBoxEsp>(max_distance.get(), default_colour.get(), tag_colours.get());
					box_esp->children.emplace_back(std::move(max_distance));
				}

				// Line ESP
				{
					auto* line_esp = player_esp->createChild<CommandList>(LOC("LINESP"));

					auto max_distance = line_esp->makeChild<CommandSlider>(LOC("MAXDIST"), { CMDNAME("lineesprange"), CMDNAME("linesprange") }, NOLABEL, 1, 100000, 1000, 50);
					line_esp->createChild<CommandPlayerLineEsp>(max_distance.get(), default_colour.get(), tag_colours.get());
					line_esp->children.emplace_back(std::move(max_distance));
				}

				player_esp->createChild<CommandDivider>(LOC("CLRS"));
				player_esp->children.emplace_back(std::move(default_colour));
				player_esp->children.emplace_back(std::move(tag_colours));
			}

			inhabs->createChild<CommandLambdaActionScript>(LOC("DELROPES"), { CMDNAME("deleteropes") }, NOLABEL, [](const Click&)
			{
				for (int i = 0; i != 100; ++i)
				{
					int tmp = i;
					PHYSICS::DELETE_ROPE(&tmp);
					PHYSICS::DELETE_CHILD_ROPE(i);
				}
			});
		}

		// Atmosphere
		{
			auto atmo = this->createChild<CommandList>(LOC("ATMO"));

			atmo->createChild<CommandClock>();
			atmo->createChild<CommandOverrideWeather>();
			atmo->createChild<CommandClouds>();
			atmo->createChild<CommandDisableSkybox>();
		}

		this->createChild<CommandWorldEditor>();

		// Water
		{
			auto water = this->createChild<CommandList>(LOC("WAT"));

			water->createChild<CommandWaterWaveBehaviour>();
			auto height = water->createChild<CommandWaterHeight>();
			height->local = water->createChild<CommandToggle>(LOC("WATZDIFFLOC"));
			water->createChild<CommandWaterStrengthOverride>();
			water->createChild<CommandWaterOpacityDifferential>();
		}

		this->createChild<CommandWorldState>();

		// Enhanced Open World
		{
			auto list = this->createChild<CommandList>(LOC("DOORS"));

			auto doors = list->createChild<CommandWorldDoors>();
			doors->blips = list->createChild<CommandLambdaToggle>(LOC("BLIPS"), CMDNAMES("doorblips"), [doors](bool, Click& click)
			{
				if (doors->m_on)
				{
					click.ensureScriptThread([doors]
					{
						doors->updateBlips();
					});
				}
			}, true);
		}

		// Watch_Dogs-Like World Hacking
		{
			auto dedsec = this->createChild<CommandList>(LOC("DEDSEC"));

			dedsec->createChild<CommandDedsec>();
			dedsec->createChild<CommandSliderPointer<CommandSlider, int>>(LOC("DEDSEC_STICKY"), { CMDNAME("dedsecsticky") }, LOC("DEDSEC_STICKY_H"), & AllEntitiesEveryTick::dedsec_deselect_delay, 0, 2000, 10);
			{
				auto passive = dedsec->createChild<CommandList>(LOC("DEDSEC_PASSIVE"));

				passive->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::dedsec_passive_reticle, LOC("SHWRETCL"), CMDNAMES("dedsecpassivereticle"));
				passive->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::dedsec_passive_line, LOC("DRWLINE"), { CMDNAME("dedsecpassiveline") });
				passive->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::dedsec_passive_box, LOC("DRWBOX"), { CMDNAME("dedsecpassivebox") });
			}
			{
				auto active = dedsec->createChild<CommandList>(LOC("DEDSEC_ACTIVE"));

				active->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::dedsec_active_reticle, LOC("SHWRETCL"), { CMDNAME("dedsecactivereticle") });
				active->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::dedsec_active_line, LOC("DRWLINE"), { CMDNAME("dedsecactiveline") });
				active->createChild<CommandTogglePointer>(&AllEntitiesEveryTick::dedsec_active_box, LOC("DRWBOX"), { CMDNAME("dedsecactivebox") });
			}
			auto colour = dedsec->createChild<CommandDedsecColour>();
			create_rainbow(colour);
			colour->command_names.at(0).append(CMDNAME("hex"));
			dedsec->createChild<CommandSliderPointer<CommandSlider, int>>(LOC("DEDSECDELAY"), CMDNAMES("dedsecdelay"), LOC("DEDSECDELAY_H"), &AllEntitiesEveryTick::phone_input_delay, 0, 60000, 10);

			// Hacks
			{
				auto hacks = dedsec->createChild<CommandList>(LOC("DEDSEC_HACK"), {}, LOC("DEDSEC_HACK_H"));

				auto general = hacks->createChild<CommandList>(LOC("DEDSEC_GENAC"));
				// Everything
				general->createChild<CommandTogglePointer>(&DedsecHack::freeze.enabled, LOC_RT(DedsecHack::freeze.name));
				general->createChild<CommandTogglePointer>(&DedsecHack::unfreeze.enabled, LOC_RT(DedsecHack::unfreeze.name));
				// Ped & Player
				general->createChild<CommandTogglePointer>(&DedsecHack::explode.enabled, LOC_RT(DedsecHack::explode.name));
				general->createChild<CommandTogglePointer>(&DedsecHack::disarm.enabled, LOC_RT(DedsecHack::disarm.name));
				// Ped & Vehicle
				general->createChild<CommandTogglePointer>(&DedsecHack::del.enabled, LOC_RT(DedsecHack::del.name));

				auto vehicle = hacks->createChild<CommandList>(LOC("DEDSEC_VEHAC"));
				vehicle->createChild<CommandTogglePointer>(&DedsecHack::destroy.enabled, LOC_RT(DedsecHack::destroy.name));
				vehicle->createChild<CommandTogglePointer>(&DedsecHack::drive.enabled, LOC_RT(DedsecHack::drive.name));
				vehicle->createChild<CommandTogglePointer>(&DedsecHack::enter.enabled, LOC_RT(DedsecHack::enter.name));
				vehicle->createChild<CommandTogglePointer>(&DedsecHack::empty.enabled, LOC_RT(DedsecHack::empty.name));
				vehicle->createChild<CommandTogglePointer>(&DedsecHack::ignite.enabled, LOC_RT(DedsecHack::ignite.name));
				vehicle->createChild<CommandTogglePointer>(&DedsecHack::slingshot.enabled, LOC_RT(DedsecHack::slingshot.name));
				vehicle->createChild<CommandTogglePointer>(&DedsecHack::menu_player_veh.enabled, LOC_RT(DedsecHack::menu_player_veh.name), {}, LOC("DEDSEC_VEHAC_H"));

				auto npc_ped = hacks->createChild<CommandList>(LOC("DEDSEC_NPCAC"));
				npc_ped->createChild<CommandTogglePointer>(&DedsecHack::burn.enabled, LOC_RT(DedsecHack::burn.name));
				npc_ped->createChild<CommandTogglePointer>(&DedsecHack::flee.enabled, LOC_RT(DedsecHack::flee.name));
				npc_ped->createChild<CommandTogglePointer>(&DedsecHack::cower.enabled, LOC_RT(DedsecHack::cower.name));
				npc_ped->createChild<CommandTogglePointer>(&DedsecHack::revive.enabled, LOC_RT(DedsecHack::revive.name));

				auto player = hacks->createChild<CommandList>(LOC("RTC_PLY"));
				player->createChild<CommandTogglePointer>(&DedsecHack::kill.enabled, LOC_RT(DedsecHack::kill.name));
				player->createChild<CommandTogglePointer>(&DedsecHack::cage.enabled, LOC_RT(DedsecHack::cage.name));
				player->createChild<CommandTogglePointer>(&DedsecHack::menu_player.enabled, LOC_RT(DedsecHack::menu_player.name), {}, LOC("DEDSEC_PLRAC_H"));
			}
		}

		this->createChild<CommandGeoGuessr>();

		// Aesthetic Light
		{
			auto* const aesthetic_light = this->createChild<CommandList>(LOC("AESLGHT"));

			auto colour = std::unique_ptr<CommandColour>(new CommandColour(aesthetic_light, LOC("CLR"), { CMDNAME("aestheticcolour") }, NOLABEL, { 1.0f, 0.0f, 1.0f }, false));
			auto placement = aesthetic_light->makeChild<CommandAestheticLightPlacement>();
			auto range = aesthetic_light->makeChild<CommandSliderFloat>(LOC("RNGE"), { CMDNAME("aestheticrange") }, NOLABEL, 0, 1000000, 900000, 100);
			auto intensity = aesthetic_light->makeChild<CommandSliderFloat>(LOC("ITNST"), { CMDNAME("aestheticintensity") }, NOLABEL, 0, 1000000, 1000, 100);
			auto shadow = aesthetic_light->makeChild<CommandSliderFloat>(LOC("DFFRCTN"), { CMDNAME("aestheticdiffraction") }, NOLABEL, -1000000, 1000000, 0, 100);
			aesthetic_light->createChild<CommandAestheticLight>(colour.get(), placement.get(), range.get(), intensity.get(), shadow.get());
			auto rainbow = aesthetic_light->makeChild<CommandAestheticRainbow>(colour.get());
			aesthetic_light->children.emplace_back(std::move(colour));
			aesthetic_light->children.emplace_back(std::move(rainbow));
			aesthetic_light->children.emplace_back(std::move(placement));
			aesthetic_light->children.emplace_back(std::move(range));
			aesthetic_light->children.emplace_back(std::move(intensity));
			aesthetic_light->children.emplace_back(std::move(shadow));
		}

		// World Border
		{
			auto* const world_border = this->createChild<CommandList>(LOC("WRLDBDR"));

			auto colour = world_border->makeChild<CommandColour>(LOC("CLR"), { CMDNAME("worldborder") }, NOLABEL, 255, 0, 255, 255);
			world_border->createChild<CommandWorldBorder>(colour.get());
			auto colour_ = colour.get();
			world_border->children.emplace_back(std::move(colour));
			create_rainbow(colour_);
			colour_->command_names.at(0).append(CMDNAME("hex"));
		}
		
		this->createChild<CommandBlackout>();
		this->createChild<CommandTrains>();
		this->createChild<CommandArWaypoint>();
		this->createChild<CommandArGps>();
	}
}
