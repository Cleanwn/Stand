#include "CommandTabGame.hpp"

#include "CommandLambdaActionScript.hpp"
#include "CommandLambdaActionScriptBasic.hpp"
#include "CommandLambdaSliderScript.hpp"
#include "CommandLambdaToggleScriptTick.hpp"
#include "CommandSliderFloat.hpp"
#include "CommandToggleableBehaviourScript.hpp"
#include "CommandTogglePointer.hpp"
#include "ComponentDrawPatch.hpp"
#include "input.hpp"
#include "ScriptGlobal.hpp"
#include "script_vm_reimpl.hpp"
#include "TransitionHelper.hpp"

#include "CommandListDisables.hpp"
#include "CommandListInfoOverlay.hpp"

#include "CommandSkipCutscene.hpp"
#include "CommandSnake.hpp"
#include "CommandTimescale.hpp"
#include "CommandAimTimescale.hpp"
#include "CommandHudColourEditor.hpp"
#include "CommandEditLabels.hpp"
#include "CommandDlcpacks.hpp"
#include "CommandListRadio.hpp"

#include "CommandListVisualSettings.hpp"
#include "CommandNightVision.hpp"
#include "CommandThermalVision.hpp"
#include "CommandTimecycleMod.hpp"
#include "CommandLodscale.hpp"
#include "CommandRenderHdOnly.hpp"
#include "CommandPotato.hpp"
#include "CommandPositionSelection.hpp"
#include "CommandStreamingPos.hpp"

#include "CommandSpCash.hpp"
#include "CommandRevealMap.hpp"
#include "CommandPreOrderBonusOverride.hpp"

#include "CommandArNotifications.hpp"

#include "CommandSleekMinimap.hpp"

#include "CommandFreecam.hpp"
#include "CommandFov.hpp"
#include "CommandUnrestrictCam.hpp"
#include "Command2ndPerson.hpp"
#include "CommandLockGameplayCamHeading.hpp"
#include "tbStareAtSky.hpp"

#include "CommandScreenshotMode.hpp"
#include "CommandRewind.hpp"
#include "CommandSetGameLanguage.hpp"
#include "CommandStutterFix.hpp"
#include "CommandKeyboardHook.hpp"
#include "CommandDebugNatives.hpp"
#include "CommandListPatches.hpp"
#include "CommandListAnalogue.hpp"
#ifdef STAND_DEBUG
#include "CommandWootingAnalog.hpp"
#endif
#include "CommandYeet.hpp"

namespace Stand
{
	CommandTabGame::CommandTabGame(CommandList* const parent)
		: CommandTab(parent, TAB_GAME, LOC("GME"), { CMDNAME("tgame"), CMDNAME("gameplay") })
	{
	}

	void CommandTabGame::populate()
	{
		if (g_gui.isRootStateFull())
		{
			this->createChild<CommandListDisables>();

			// Camera
			{
				auto camera = this->createChild<CommandList>(LOC("PLCMNT_CAM"));

				// Freecam
				{
					auto freecam = camera->createChild<CommandList>(LOC("FC"), {}, LOC("FC_H"));

					auto speed = freecam->makeChild<CommandSliderFloat>(LOC("MOVSPD"), { CMDNAME("freecamspeed") }, NOLABEL, 1, 1000000, 15, 5);
					auto sprint_speed = freecam->makeChild<CommandSliderFloat>(LOC("SPRNTMUL"), { CMDNAME("freecamsprintmultiplier") }, LIT(LANG_FMT("SPRNTMUL_H", Input::vk_to_string(VK_SPACE))), 1, 1000000, 800, 20);
					auto tp_on_disable = freecam->makeChild<CommandToggleNoCorrelation>(LOC("FC_DTP"), { CMDNAME("freecamtp") }, LOC("FC_DTP_H"));
					auto ignore_pitch = freecam->makeChild<CommandToggleNoCorrelation>(LOC("FC_NZ"), { CMDNAME("freecamignorepitch") }, LOC("FC_NZ_H"));
					auto minimap_follows = freecam->makeChild<CommandToggleNoCorrelation>(LOC("FC_MF"), { CMDNAME("freecamminmap") }, NOLABEL, true);
					auto show_button_instructions = freecam->makeChild<CommandToggleNoCorrelation>(LOC("SHWBI"), { CMDNAME("freecambuttoninstructions") }, NOLABEL, true);
					auto movement = freecam->makeChild<CommandToggleNoCorrelation>(LOC("MOVCTRL"), { CMDNAME("freecammovement"), CMDNAME("freecamovement") }, LOC("MOVCTRL_FC"), true);
					g_tb_freecam.speed = speed.get();
					g_tb_freecam.sprint_speed = sprint_speed.get();
					g_tb_freecam.ignore_pitch = ignore_pitch.get();
					g_tb_freecam.minimap_follows = minimap_follows.get();
					g_tb_freecam.show_button_instructions = show_button_instructions.get();
					g_tb_freecam.movement = movement.get();
					freecam->createChild<CommandFreecam>(tp_on_disable.get());
					freecam->children.emplace_back(std::move(speed));
					freecam->children.emplace_back(std::move(sprint_speed));
					freecam->children.emplace_back(std::move(tp_on_disable));
					freecam->children.emplace_back(std::move(ignore_pitch));
					freecam->children.emplace_back(std::move(minimap_follows));
					freecam->children.emplace_back(std::move(show_button_instructions));
					freecam->children.emplace_back(std::move(movement));
				}

				// FOV
				{
					auto fov = camera->createChild<CommandList>(LOC("FOV"));

					g_tb_custom_fov.tp_in_vehicle = fov->createChild<CommandFov>(LOC("FOV_TP_VEH"), { CMDNAME("fovtpinveh") });
					g_tb_custom_fov.tp_on_foot = fov->createChild<CommandFov>(LOC("FOV_TP_ONF"), { CMDNAME("fovtponfoot") });
					g_tb_custom_fov.fp_in_vehicle = fov->createChild<CommandFov>(LOC("FOV_FP_VEH"), { CMDNAME("fovfpinveh") });
					g_tb_custom_fov.fp_on_foot = fov->createChild<CommandFov>(LOC("FOV_FP_ONF"), { CMDNAME("fovfponfoot") });
					g_tb_custom_fov.aiming = fov->createChild<CommandFov>(LOC("FOV_AIM"), { CMDNAME("fovaiming") });
					g_tb_custom_fov.sniping = fov->createChild<CommandFov>(LOC("FOV_SNIP"), { CMDNAME("fovsniping") });
				}

				camera->createChild<CommandLambdaSliderScript<CommandSliderFloat>>(LOC("CAMDIST"), { CMDNAME("cameradistance") }, NOLABEL, -1000000, 1000000, 100, 10, [](int value, const Click&)
				{
					g_gui.user_defined_camera_distance_multiplier = (float)value / 100.0f;
					g_gui.set_camera_distance_from_character_multiplier_all_things_considered();
				});
				camera->createChild<CommandUnrestrictCam>();
				camera->createChild<Command2ndPerson>();
				camera->createChild<CommandLockGameplayCamHeading>();
				camera->createChild<CommandToggleableBehaviourScript>(&g_tb_stare_at_sky, LOC("ACCAM"), { CMDNAME("anticrashcamera") });
			}

			// Rendering
			{
				auto rendering = this->createChild<CommandList>(LOC("RDRG"));

				rendering->createChild<CommandListVisualSettings>();
				rendering->createChild<CommandNightVision>();
				rendering->createChild<CommandThermalVision>();
				rendering->createChild<CommandTimecycleMod>();
				rendering->createChild<CommandLodscale>();
				rendering->createChild<CommandRenderHdOnly>();
				rendering->createChild<CommandPotato>();
				rendering->createChild<CommandTogglePointer>(&g_comp_drawpatch.block_all, LOC("BLKALLDRW"), { CMDNAME("norender"), CMDNAME("nodraw") });
				{
					auto streaming_pos = rendering->createChild<CommandList>(LOC("LKSTRMFCS"));

					auto pos_sel = streaming_pos->makeChild<CommandPositionSelection>(LOC("LKDPOS"), { CMDNAME("lockedstreamingpos") });
					streaming_pos->createChild<CommandStreamingPos>(pos_sel.get());
					streaming_pos->children.emplace_back(std::move(pos_sel));
				}
			}

			// Info Overlay
			{
				this->createChild<CommandListInfoOverlay>();
			}

			this->createChild<CommandHudColourEditor>();

			// Edit Labels
			{
				this->createChild<CommandEditLabels>();
			}

			this->createChildWithBuilder<CommandDlcpacks>();

			this->createChild<CommandListRadio>();

			// Unlocks
			{
				auto unlocks = this->createChild<CommandList>(LOC("UNLKS"));

				// Set Story Mode Cash
				{
					auto spcash = unlocks->createChild<CommandList>(LOC("SPCSH"), CMDNAMES(), LOC("SPCSH_H"));

					spcash->children.emplace_back(std::make_unique<CommandSpCash>(spcash, LOC("SCHR_M"), CMDNAMES("spcashmichael"), ATSTRINGHASH("SP0_TOTAL_CASH")));
					spcash->children.emplace_back(std::make_unique<CommandSpCash>(spcash, LOC("SCHR_F"), CMDNAMES("spcashfranklin"), ATSTRINGHASH("SP1_TOTAL_CASH")));
					spcash->children.emplace_back(std::make_unique<CommandSpCash>(spcash, LOC("SCHR_T"), CMDNAMES("spcashtrevor"), ATSTRINGHASH("SP2_TOTAL_CASH")));
				}

				unlocks->createChild<CommandLambdaActionScriptBasic>(LOC("UNLKONL"), CMDNAMES("skipprologue", "skiprologue"), NOLABEL, [](const Click&)
				{
					TransitionHelper::skipPrologue();
				});
				unlocks->createChild<CommandLambdaActionScriptBasic>(LOC("UNLKALLACHIV"), CMDNAMES("unlockachievements"), NOLABEL, [](const Click&)
				{
					for (int i = 1; i <= 77; i++)
					{
						PLAYER::GIVE_ACHIEVEMENT_TO_PLAYER(i);
					}
				});
				unlocks->createChild<CommandRevealMap>();
				unlocks->createChild<CommandPreOrderBonusOverride>();
			}

			// AR Notifications
			{
				auto arnotifs = this->createChild<CommandList>(LOC("ARNOTIFS"));

				auto main = arnotifs->createChild<CommandArNotifications>();
				main->debug_mode = arnotifs->createChild<CommandToggle>(LOC("DBGMODE"), { CMDNAME("arnotifsdebug") });
				main->dynamic_scaler = arnotifs->createChild<CommandSliderFloat>(LOC("DYNSCLE"), { CMDNAME("arnotifsdynamicscale") }, NOLABEL, 0, 100000, 140, 10);
				main->static_scaler = arnotifs->createChild<CommandSliderFloat>(LOC("STATSCLE"), { CMDNAME("arnotifsstaticscale") }, NOLABEL, 0, 100000, 1000, 100);
				main->dynamic_scaling = arnotifs->createChild<CommandToggle>(LOC("DYNSCLG"), { CMDNAME("arnotifsdynamicscaling") }, NOLABEL, true);
				main->rotation_mode = arnotifs->createChild<CommandListSelect>(LOC("ROTMODE"), { CMDNAME("arnotifsrotation") }, NOLABEL, std::vector<CommandListActionItemData>{
					{ 0, LOC("ARNOTIFS_ROTMODE_0") },
					{ 1, LOC("ARNOTIFS_ROTMODE_1") },
					{ 2, LOC("ARNOTIFS_ROTMODE_2") },
					{ 3, LOC("ARNOTIFS_ROTMODE_3") },
				}, 0);
				main->horizontal_offset = arnotifs->createChild<CommandSliderFloat>(LOC("HOROFF"), { CMDNAME("arnotifshorizontaloffset"), CMDNAME("arnotifsplayeroffset") }, NOLABEL, -10000, 10000, -100, 5);
				main->vertical_offset = arnotifs->createChild<CommandSliderFloat>(LOC("VERTOFF"), { CMDNAME("arnotifsverticaloffset"), CMDNAME("arnotifszoffset") }, NOLABEL, -10000, 10000, 0, 5);
				main->depth_offset = arnotifs->createChild<CommandSliderFloat>(LOC("DPHOFF"), { CMDNAME("arnotifsdepthoffset") }, NOLABEL, -10000, 10000, 0, 10);
				main->fp_horizontal_offset = arnotifs->createChild<CommandSliderFloat>(LOC("FPHOROFF"), { CMDNAME("arnotifsfphoriztonaloffset") }, NOLABEL, -10000, 10000, -205, 5);
				main->fp_vertical_offset = arnotifs->createChild<CommandSliderFloat>(LOC("FPVERTOFF"), { CMDNAME("arnotifsfpverticaloffset") }, NOLABEL, -10000, 10000, -5, 5);
				main->fp_depth_offset = arnotifs->createChild<CommandSliderFloat>(LOC("FPDPHOFF"), { CMDNAME("arnotifsfpdepthoffset") }, NOLABEL, -10000, 10000, 390, 10);
				main->top_offset = arnotifs->createChild<CommandSliderFloat>(LOC("RESVRDTOPSPC"), { CMDNAME("arnotifsreservedtopspace"), CMDNAME("arnotifstopoffset") }, NOLABEL, 0, 100, 40);
				main->ensure_text_readability = arnotifs->createChild<CommandToggle>(LOC("ENSTXTREAD"), { CMDNAME("arnotifsbackface") }, NOLABEL, true);
			}

			// Minimap
			{
				auto minimap = this->createChild<CommandList>(LOC("MINIMAP"));

				minimap->createChild<CommandSleekMinimap>();
				minimap->createChild<CommandLambdaToggleScriptTick>(LOC("DNWINVI"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("inviteindicator")), LOC("DNWINVI_H"), [] { ScriptGlobal(GLOBAL_NEW_INVITES).set<int>(0); });
			}

			this->createChild<CommandLambdaActionScript>(LOC("T_M_C"), combineCommandNames({ CMDNAME("clear"), CMDNAME("remove"), CMDNAME("delete") }, CMDNAME("notifications")), NOLABEL, [](const Click&)
			{
				HUD::THEFEED_PAUSE();
				HUD::THEFEED_CLEAR_FROZEN_POST();
				HUD::THEFEED_FLUSH_QUEUE();
				HUD::THEFEED_RESUME();
			});
			this->createChild<CommandSkipCutscene>();
			this->createChild<CommandSnake>();
			auto timescale = this->makeChild<CommandTimescale>();
			auto aimtimescale = this->makeChild<CommandAimTimescale>(timescale.get());
			timescale->aimtimescale = aimtimescale.get();
			this->children.emplace_back(std::move(timescale));
			this->children.emplace_back(std::move(aimtimescale));
			this->createChild<CommandScreenshotMode>();
			this->createChild<CommandRewind>();
			this->createChild<CommandSetGameLanguage>();
			this->createChild<CommandStutterFix>();
			this->createChild<CommandKeyboardHook>();
			this->createChild<CommandDebugNatives>();
		}

		this->createChild<CommandListPatches>();

		this->createChild<CommandListAnalogue>();
#ifdef STAND_DEBUG
		this->createChild<CommandWootingAnalog>();
#endif

		// Early Inject Enhancements
		{
			auto ej = this->createChild<CommandList>(LOC("EJ"), {}, LOC("EJ_H"));

			ej->createChild<CommandSliderFloat>(LOC("GMEPOOL"), { CMDNAME("gamepoolmultiplier") }, LOC("GMEPOOL_H"), 100, 700, 100, 100);
			ej->createChild<CommandSlider>(LOC("MEMPOOL"), { CMDNAME("mempool") }, NOLABEL, 488, 2147, 488, 600);
			ej->createChild<CommandToggleNoCorrelation>(LOC("SETHIPRIO2"));
			ej->createChild<CommandToggleNoCorrelation>(LOC("SPDUPSTRUP"), {}, NOLABEL, true);
			ej->createChild<CommandToggleNoCorrelation>(LOC("SKPGMEITR2"), {}, NOLABEL, true);
		}

#if SVM_REIMPL
		this->createChild<CommandTogglePointer>(&g_use_script_vm_reimpl, LOC("SVMR"), CMDNAMES_OBF("svmreimpl"), LOC("SVMR_H"));
#endif

		this->createChild<CommandYeet>();
	}
}
