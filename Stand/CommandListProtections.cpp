#include "CommandListProtections.hpp"

#include <fmt/core.h>

#include <soup/ObfusString.hpp>

#include "atStringHash.hpp"
#include "ColoadMgr.hpp"
#include "CommandDivider.hpp"
#include "CommandListSelectUnlessFriend.hpp"
#include "CommandPlayer.hpp"
#include "CommandReadonlyLink.hpp"
#include "CommandTogglePointer.hpp"
#include "ComponentDrawPatch.hpp"
#include "ComponentNetcode.hpp"
#include "Hooking.hpp"
#include "main.hpp"

#include "CommandFlowEventGlobal.hpp"

#include "CommandModelBlacklist.hpp"
#include "CommandNoVoteKick.hpp"
#include "CommandBlockBlockJoin.hpp"
#include "CommandBlockBlame.hpp"
#include "CommandAntiEntitySpam.hpp"
#include "CommandScriptErrorRecovery.hpp"
#if HAVE_BREAKUP_KICK
#include "CommandHostKickKarma.hpp"
#endif
#include "CommandDesyncKickKarma.hpp"
#include "CommandLoveLetterLube.hpp"
#include "CommandNoBeast.hpp"
#include "CommandNoModPop.hpp"
#include "CommandForceRelay.hpp"

namespace Stand
{
	CommandListProtections::CommandListProtections(CommandList* const parent)
		: CommandListWarning(parent, LOC("PTX"), ATSTRINGHASH("PTX_W"), CMDNAMES_OBF("protections", "protex"))
	{
		// Network Events
		{
			auto* net_events = this->createChild<CommandList>(LOC("PTX_EVTS"));

			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_CRASH);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_KICK);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_INVALID, LOC("PTX_INV_H"));
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_UNUSUAL, LOC("PTX_WEIRD_H"));
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_APTINV);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_FORCECAMFRONT);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_TRIGGERDEFEND);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_STARTFMMISSION);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_STARTFMMISSION_NCEO);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_INTERIORWARP);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_INTERIORWARP_NCEO);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_GIVECOLLE);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_GIVECOLLE_NCEO);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_CEOKICK);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_INFLOAD);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::NE_INFRING);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_CAYO_INV);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_SENDTOJOB);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::NE_VEHTAKEOVER, LOC("PTX_DRVCTRL_H"));
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_DISABLEDRIVINGVEHICLES);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_PVKICK);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_INTERIORKICK);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SE_FREEZE);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::NE_CAMSHAKE);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(LOC("EXPSPAM"), FlowEvent::NE_EXPSPAM, LOC("EXPSPAM_H"));
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(LOC("RAGDOLLEVT"), FlowEvent::NE_RAGDOLL);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::NE_SUDDENDEATH);
			net_events->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::NE_DISARM);

			// Raw Network Events
			{
				auto raw_net_events = net_events->createChild<CommandListWarning>(LOC("PTX_RAWNET"), ATSTRINGHASH("PTX_RAW_W"));

				raw_net_events->createChildWithBuilder<CommandFlowEventGlobal>(LOC("PTX_NET_ANY"), FlowEvent::NE_ANY);
				raw_net_events->createChildWithBuilder<CommandFlowEventGlobal>(SCRIPTED_GAME_EVENT);
				for (const auto& event_id : net_event_ids)
				{
					if (event_id != SCRIPTED_GAME_EVENT && event_id != NETWORK_INCREMENT_STAT_EVENT)
					{
						raw_net_events->createChildWithBuilder<CommandFlowEventGlobal>(get_net_event_name(event_id), event_id);
					}
				}
			}
		}

		// Detections
		if (g_gui.isRootStateFull())
		{
			auto detections = this->createChild<CommandList>(LOC("DTS"));

			// Classifications
			{
				auto classifications = detections->createChild<CommandList>(LOC("CLSFNS"));

				classifications->createChildWithBuilder<CommandFlowEventGlobal>(LOC("CLSFN_ANY"), FlowEvent::CLSFN_ANY);
				classifications->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::CLSFN_MOD);
				classifications->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::CLSFN_AMN);
#if HAS_MODAMN
				classifications->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::CLSFN_MODAMN);
#endif
				classifications->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::CLSFN_LM);
				classifications->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::CLSFN_PM);
			}

			detections->createChildWithBuilder<CommandFlowEventGlobal>(LOC("DT_ANY"), FlowEvent::MOD_ANY);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_ID_STAND);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_ID_STAND_COLOADING);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_ID_OTHER);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_RAC);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_HEALTH);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_LONG_DEATH);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_LONG_OTR);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_ARMOUR);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_PRESENCESPOOFING, LOC("MDDRDT_PRES_H"));
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_ADMINRID);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_T2IP);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_HOSTKN_AGGRESSIVE);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_HOSTKN_SWEET);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_HOSTKN_HANDICAP);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_SPFDHOSTKN);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_MODEL);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_GODATK);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_DMGMUL);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_FROG);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_RSDEV);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_RSQA);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_CHEAT);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_RP2);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_RP3);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_RP4);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_NONET, LOC("MDDRT_NONET_H"));
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_DRIVEB);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_SILENTCHAT);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_BOUNTYSPAM, LOC("MDDRT_BOUNTYSPAM_H"));
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_SUSSYBOUNTY, LOC("MDDRT_SUSSYBOUNTY_H"));
#if HAS_EXPBLAME
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_EXPBLAME, LOC("MDDRT_EXPBLAME_H"));
#endif
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_BADEXPLOSION, LOC("MDDRT_EXPL_H"));
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_ORGNAME);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_CAYO);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_LLLUBE);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_MISC);
#ifdef STAND_DEBUG
			detections->createChild<CommandDivider>(LIT("Debug Build Shit"));
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_INCONGRUENCE_1, NOLABEL, CMDFLAG_FEATURELIST_SKIP);
			detections->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::MOD_INCONGRUENCE_2, NOLABEL, CMDFLAG_FEATURELIST_SKIP);
#endif

			// Anti-Detection
			{
				auto anti_detection = this->createChild<CommandList>(LOC("ADT"));

				anti_detection->createChild<CommandTogglePointer>(&g_hooking.block_outgoing_godmode_flag, LOC("FLAG_GOD"));
				anti_detection->createChild<CommandTogglePointer>(&g_hooking.block_outgoing_veh_godmode_flag, LOC("FLAG_VEHGOD"));
				anti_detection->createChild<CommandTogglePointer>(&g_hooking.block_outgoing_spectating_flag, LOC("FLAG_SPEC"), {}, LOC("ADT_SPEC_H"));
				anti_detection->createChild<CommandTogglePointer>(&g_hooking.block_outgoing_damage_mod, LOC("DMG"));
				anti_detection->createChild<CommandTogglePointer>(&g_hooking.block_outgoing_modded_health, LOC("MDDRDT_HLTH"));
#if HAVE_SUPERJUMP_ANTI_DETECTION
				anti_detection->createChild<CommandTogglePointer>(&g_hooking.block_outgoing_super_jump_flag, LOC("FROG"));
#endif
			}
		}

		// Syncs
		{
			auto syncs = this->createChild<CommandList>(LOC("PTX_S"));

			syncs->createChild<CommandTogglePointer>(&g_hooking.block_outgoing_syncs_to_host, LOC("OUTSBH"), {}, LOC("OUTSBH_H"));
			syncs->createChild<CommandModelBlacklist>();
			syncs->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_CAGE, LOC("PTX_BEWARE_RESYNCH"));
			syncs->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_WOS, LOC("PTX_BEWARE_RESYNCH"));
			syncs->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_IMS, LOC("PTX_BEWARE_RESYNCH"));
			syncs->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_ATTACH, LOC("PTX_BEWARE_RESYNCH"));

			syncs->createChild<CommandDivider>(LOC("PTX_RAWSYNC"));

			{
				auto syncin = syncs->createChild<CommandListWarning>(LOC("PTX_INC"), ATSTRINGHASH("PTX_RAW_W"));

				syncin->createChildWithBuilder<CommandFlowEventGlobal>(LOC("PTX_SI_ANY"), FlowEvent::SYNCIN_ANY);
				syncin->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_CLONE_CREATE);
				syncin->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_CLONE_UPDATE);
				syncin->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_CLONE_DELETE);
				syncin->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_ACK_CLONE_CREATE);
				syncin->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_ACK_CLONE_UPDATE);
				syncin->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCIN_ACK_CLONE_DELETE);
			}

			{
				auto syncout = syncs->createChild<CommandListWarning>(LOC("PTX_OUT"), ATSTRINGHASH("PTX_RAW_W"));
				syncout->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCOUT_CLONE_CREATE);
				syncout->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCOUT_CLONE_UPDATE);
				syncout->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SYNCOUT_CLONE_DELETE);
			}
		}

		// Text Messages
		{
			auto sms_reactions = this->createChild<CommandList>(LOC("SMSS"));

			sms_reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("CHT_ANY"), FlowEvent::SMS_ANY);
			sms_reactions->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SMS_AD);
			sms_reactions->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SMS_PROFANITYBYPASS);
		}

		// Session Script Start
		{
			auto script_start = this->createChild<CommandList>(LOC("SESSSCRSTRT"));

			script_start->createChildWithBuilder<CommandFlowEventGlobal>(LOC("SESSSCRSTRT_ANY"), FlowEvent::SCRLAUNCH_ANY);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_NOCAT);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_FE);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_AG);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_OFM);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_GRIEF);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_SERV);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_FOIM);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_PS);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_STRIKE);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_DPSSV);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_DARTS);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_IMPDM);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_SLASHER);
			script_start->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::SCRLAUNCH_CUT);
		}

		// Host-Controlled Kicks
		{
			auto hcks = this->createChild<CommandList>(LOC("PTX_HCK"));

#if HAVE_BREAKUP_KICK
			hcks->createChild<CommandHostKickKarma>();
#endif
			hcks->createChild<CommandTogglePointer>(&g_comp_netcode.host_kick_protex, LOC("PTX_HK"), CMDNAMES_OBF("lessenhostkicks"), LOC("PTX_HK_H"));

			if (g_gui.isRootStateFull())
			{
				hcks->createChild<CommandTogglePointer>(&g_hooking.toast_love_letter_kick, LOC("LLKT"), CMDNAMES_OBF("notifyloveletter", "notifydesync"));
				hcks->createChild<CommandDesyncKickKarma>();
				hcks->createChild<CommandLoveLetterLube>();
			}
		}

#if false
		// Buttplug Kick Reactions
		{
			auto buttplug = this->createChild<CommandList>(LOC("BPKR"), {}, LOC("GEN_H_HOST"));

			{
				auto myself = buttplug->createChild<CommandList>(LOC("MYSLF"));

				g_comp_netcode.buttplug_myself_toasts.populateList(myself);
				myself->createChild<CommandTogglePointer>(&g_comp_netcode.buttplug_myself_block, LOC("PTX_NET_R_B"));
			}

			{
				auto someoneelse = buttplug->createChild<CommandList>(LOC("SOMELS"));

				g_comp_netcode.buttplug_someoneelse_toasts.populateList(someoneelse);
				someoneelse->createChild<CommandListSelectUnlessFriend>(&g_comp_netcode.buttplug_someoneelse_block);
			}
		}
#endif

#if HAVE_BREAKUP_KICK
		// Breakup Kick Reactions
		{
			auto breakup = this->createChild<CommandList>(LOC("BRKUR"), {}, LOC("BRKUR_H"));

			g_comp_netcode.breakup_someoneelse_toasts.populateList(breakup);
			breakup->createChild<CommandListSelectUnlessFriend>(&g_comp_netcode.breakup_someoneelse_block, LOC("PTX_NET_R_B"), LOC("GEN_H_HOST"));
			breakup->createChild<CommandListSelectUnlessFriend>(&g_comp_netcode.breakup_someoneelse_karma, LOC("KARMA"), LOC("GEN_H_NHOST"));
		}

		// Knockoff Breakup Kick Reactions
		{
			auto badbreakup = this->createChild<CommandList>(LOC("KBKR"));

			{
				auto myself = badbreakup->createChild<CommandList>(LOC("MYSLF"));

				g_comp_netcode.badbreakup_myself_toasts.populateList(myself);
				myself->createChild<CommandTogglePointer>(&g_comp_netcode.badbreakup_myself_karma, LOC("KARMA"));
			}

			{
				auto someoneelse = badbreakup->createChild<CommandList>(LOC("SOMELS"));

				g_comp_netcode.badbreakup_someoneelse_toasts.populateList(someoneelse);
				someoneelse->createChild<CommandDivider>(LOC("RTC_PLY"));
				someoneelse->createChild<CommandListSelectUnlessFriend>(&g_comp_netcode.badbreakup_someoneelse_kick, LOC("PLYKCK"));
			}
		}
#endif

		// Presence Events
		{
			auto pres = this->createChild<CommandList>(LOC("PRESES"));

			pres->createChild<CommandTogglePointer>(&g_hooking.block_friend_stat_notifications, LOC("BFSTN"), combineCommandNames(CMDNAMES_OBF("no", "block", "disable"), CMDNAME_OBF("friendstatnotifications")), LOC("BFSTN_H"));
			pres->createChild<CommandTogglePointer>(&g_hooking.block_job_invites, LOC("BJI"), combineCommandNames(CMDNAMES_OBF("no", "block", "disable"), CMDNAME_OBF("jobinvites")));
		}

		// Pickups
		{
			auto* pickups = this->createChild<CommandList>(LOC("PUPS"));

			pickups->createChildWithBuilder<CommandFlowEventGlobal>(LOC("PTX_APUP"), FlowEvent::PUP_ANY);
			pickups->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::PUP_CASH);
			pickups->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::PUP_RP);
			pickups->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::PUP_INVALID, LOC("PTX_IPUP_H"));
		}

		// Co-loading
		{
			auto* coloading = this->createChild<CommandList>(LOC("COLOAD"));

			coloading->createChild<CommandTogglePointer>(&g_hooking.ignore_crash_n1_when_coloading, LIT(LANG_FMT("COLOAD_DPTX", std::move(std::string(LANG_GET("PTX_CRSHE")).append(soup::ObfusString(" (N1)").str())))));
			coloading->createChild<CommandTogglePointer>(&g_hooking.ignore_crash_c1_when_coloading, LIT(LANG_FMT("COLOAD_DPTX", std::move(std::string(LANG_GET("PTX_CRSHE")).append(soup::ObfusString(" (C1)").str())))));
			coloading->createChild<CommandTogglePointer>(&g_hooking.ignore_crash_c3_when_coloading, LIT(LANG_FMT("COLOAD_DPTX", std::move(std::string(LANG_GET("PTX_CRSHE")).append(soup::ObfusString(" (C3)").str())))));
			coloading->createChild<CommandTogglePointer>(&g_hooking.ignore_crash_u1_when_coloading, LIT(LANG_FMT("COLOAD_DPTX", std::move(std::string(LANG_GET("PTX_CRSHE")).append(soup::ObfusString(" (U1)").str())))));
			coloading->createChild<CommandTogglePointer>(&g_hooking.ignore_crash_u2_when_coloading, LIT(LANG_FMT("COLOAD_DPTX", std::move(std::string(LANG_GET("PTX_CRSHE")).append(soup::ObfusString(" (U2)").str())))));
			coloading->createChild<CommandTogglePointer>(&g_hooking.ignore_crash_cs_when_coloading, LIT(LANG_FMT("COLOAD_DPTX", std::move(std::string(LANG_GET("PTX_CRSHE")).append(soup::ObfusString(" (CS)").str())))));
			coloading->createChild<CommandTogglePointer>(&g_hooking.ignore_crash_sr_when_coloading, LIT(LANG_FMT("COLOAD_DPTX", std::move(std::string(LANG_GET("PTX_CRSHE")).append(soup::ObfusString(" (SR)").str())))));
			coloading->createChild<CommandTogglePointer>(&g_hooking.ignore_crash_t6_when_coloading, LIT(LANG_FMT("COLOAD_DPTX", std::move(std::string(LANG_GET("PTX_CRSHE")).append(soup::ObfusString(" (T6)").str())))));
		}

		if (g_gui.isRootStateFull())
		{
			// Block Bailing
			{
				auto bail_block = this->createChild<CommandList>(LOC("PTX_NB"));

				bail_block->createChild<CommandTogglePointer>(&g_hooking.block_bail_rid, LOC("PTX_NB_1"));
				bail_block->createChild<CommandTogglePointer>(&g_hooking.block_bail_crew, LOC("PTX_NB_2"));
				bail_block->createChild<CommandTogglePointer>(&g_hooking.block_bail_spectating, LOC("PTX_NB_SPC"), {}, LOC("PTX_NB_SPC_H"));
				bail_block->createChild<CommandTogglePointer>(&g_hooking.block_bail_other, LOC("PTX_NB_O"), {}, LOC("PTX_NB_O_H"));
			}
		}

		this->createChild<CommandReadonlyLink>(LOC("PTXGDE"), soup::ObfusString("https://stand.sh/help/protections"));

		if (g_gui.isRootStateFull())
		{
			this->createChild<CommandBlockBlame>();
			this->createChild<CommandNoVoteKick>();

			// Block Join Karma
			{
				auto list = this->createChild<CommandList>(LOC("PTX_BJ"), {}, LOC("PTX_BJ_H"));
				list->createChild<CommandBlockBlockJoin>();
				list->createChild<CommandDivider>(LOC("NOTFS"));
				CommandBlockBlockJoin::toast_config.populateList(list);
			}
		}
		{
			auto aes = this->createChild<CommandList>(LOC("PTX_ENTSPAM"));

			aes->createChild<CommandAntiEntitySpam>();
			aes->createChild<CommandTogglePointer>(&AntiEntitySpam::use_cam, LOC("PTX_ENTSPAM_USECAM"));
		}
		this->createChild<CommandTogglePointer>(&g_comp_drawpatch.enabled, LOC("PTX_DRAW"), CMDNAMES_OBF("drawpatch"), LOC("PTX_DRAW_H"));

		// Delete Stale Objects
		{
			auto list = this->createChild<CommandList>(LOC("PTX_DELSTALE"), {}, LOC("PTX_DELSTALE_H"));
			list->createChild<CommandTogglePointer>(&g_hooking.erase_objects_with_no_mi, LOC("PTX_DELSTALE"), {}, LOC("PTX_DELSTALE_H"));
			list->createChild<CommandDivider>(LOC("NOTFS"));
			g_hooking.erase_objects_with_no_mi_notify.populateList(list);
		}

		this->createChild<CommandNoBeast>();
		this->createChild<CommandNoModPop>();

		this->createChild<CommandScriptErrorRecovery>();

		// Block Player Model Swaps
		{
			auto list = this->createChild<CommandList>(LOC("NPLYMDLSWP"));
			list->createChild<CommandTogglePointer>(&g_hooking.block_player_model_swaps, LOC("NPLYMDLSWP"), {}, LOC("NPLYMDLSWP_H"));
			list->createChild<CommandDivider>(LOC("XCLDS"));
			g_hooking.block_player_model_swaps_excludes.populateList(list);
		}

		if (g_gui.isRootStateFull())
		{
			this->createChild<CommandForceRelay>();
		}
	}

	bool CommandListProtections::shouldShowWarning() const
	{
		return ColoadMgr::coloading_with_any_menu;
	}
}
