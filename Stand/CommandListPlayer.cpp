#include "CommandListPlayer.hpp"

#include "CommandInputText.hpp"
#include "CommandDivider.hpp"
#include "CommandPlayerSlider.hpp"
#include "FlowEventReaction.hpp"
#include "IncrementableStat.hpp"
#include "Tunables.hpp"

#include "CommandPlayerInfo.hpp"
#include "CommandPlayerClassifier.hpp"

#include "CommandPlayerTeleport.hpp"
#include "CommandPlayerTeleportIntoVeh.hpp"
#include "CommandPlayerVisit.hpp"

#include "CommandPlayerTpToMe.hpp"
#include "CommandPlayerTpToPlayer.hpp"
#include "CommandPlayerTpToMyWp.hpp"
#include "CommandPlayerTpToMyObjective.hpp"
#include "CommandPlayerTpToTheirWp.hpp"
#include "CommandPlayerAptList.hpp"
#include "CommandPlayerCasinoTp.hpp"

#include "CommandPlayerVehicleUpgrade.hpp"
#include "CommandPlayerVehRepair.hpp"
#include "CommandPlayerVehicleGodmode.hpp"
#include "CommandPlayerVehicleNoLockon.hpp"
#include "CommandPlayerVehEnginePowerMultiplier.hpp"
#include "CommandPlayerGiveCollectibles.hpp"
#include "CommandPlayerObtainFastRun.hpp"
#include "CommandPlayerRelinquishFastRun.hpp"
//#include "CommandPlayerGift.hpp"
#include "CommandPlayerSetRank.hpp"
#include "CommandPlayerCeopay.hpp"
#include "CommandPlayerRp.hpp"
#include "CommandPlayerDropFigurines.hpp"
#include "CommandPlayerDropCards.hpp"
#include "CommandPlayerAutoheal.hpp"
#include "CommandPlayerBail.hpp"
#include "CommandPlayerOtr.hpp"
#include "CommandPlayerSnack.hpp"
#include "CommandPlayerExplosiveHits.hpp"
#include "CommandPlayerImpactParticles.hpp"
#include "CommandPlayerGiveSh.hpp"

#include "CommandPlayerChatMock.hpp"
#include "CommandPlayerChatOwoify.hpp"
#include "CommandPlayerSendPM.hpp"

#include "CommandPlayerArm.hpp"
#include "CommandPlayerAmmo.hpp"
#include "CommandPlayerParachute.hpp"
#include "CommandPlayerDisarm.hpp"

#include "CommandPlayerAttackers.hpp"

#include "CommandPlayerVehKill.hpp"
#include "CommandPlayerVehFlip.hpp"
#include "CommandPlayerVehSpin.hpp"
#include "CommandPlayerVehEMP.hpp"
#include "CommandPlayerVehDelete.hpp"
#include "CommandPlayerVehDestroyProp.hpp"
#include "CommandPlayerVehBreakTailBoom.hpp"
#include "CommandPlayerVehPopTyres.hpp"
#include "CommandPlayerVehSlingshot.hpp"
#include "CommandPlayerVehLockDoors.hpp"
#include "CommandPlayerVehDowngrade.hpp"
#include "CommandPlayerVehTurnAround.hpp"
#include "CommandPlayerVehSlipperyTyres.hpp"
#include "CommandPlayerVehRemoveDoors.hpp"
#include "CommandPlayerVehIgnite.hpp"
#include "CommandPlayerVehDetachWing.hpp"
#include "CommandPlayerVehDetachWheel.hpp"
#include "CommandPlayerVehKick.hpp"
#include "CommandPlayerInteriorKick.hpp"
#include "CommandPlayerRam.hpp"
#include "CommandPlayerRamSpeed.hpp"
#include "CommandPlayerSetWantedLevel.hpp"
#include "CommandPlayerFreeze.hpp"
#include "CommandPlayerFakeMoneyDrop.hpp"
#include "CommandPlayerBlockPassiveMode.hpp"
#include "CommandPlayerForceCamFront.hpp"
//#include "CommandPlayerTransactionError.hpp"
#include "CommandPlayerRagdoll.hpp"
#include "CommandPlayerSoundSpam.hpp"
#include "CommandPlayerShakeCam.hpp"
#include "CommandGravitateNPCs.hpp"
#include "CommandAggressiveNPCs.hpp"
#include "CommandPlayerMuggerLoop.hpp"
#include "CommandPlayerBeast.hpp"
#include "CommandPlayerKill.hpp"
#include "CommandPlayerCage.hpp"
#include "CommandPlayerSendtojob.hpp"
#include "CommandPlayerDisableDrivingVehicles.hpp"
#include "CommandPlayerCeoKick.hpp"
#include "CommandPlayerInfiniteLoading.hpp"
#include "CommandPlayerInfiniteRing.hpp"
#include "CommandPlayerForceFmMission.hpp"
#include "CommandPlayerTriggerDefend.hpp"

#include "CommandPlayerExplode.hpp"
//#include "CommandPlayerBlame.hpp"
#include "CommandPlayerBounty.hpp"
#include "CommandPlayerLoopBounty.hpp"

#include "CommandListSelectParticle.hpp"
#include "CommandPlayerParticleSpam.hpp"

#include "CommandPlayerNotifyRemoved.hpp"
#include "CommandPlayerNotifyStolen.hpp"
#include "CommandPlayerNotifyBanked.hpp"
#include "CommandPlayerNotifySpam.hpp"

// Kinda useless since it's only usable on friends and members of the same non-Rockstar crew.
#define SMS_OPTIONS false

#if SMS_OPTIONS
#include "CommandPlayerSmsText.hpp"
#include "CommandPlayerSmsSend.hpp"
#include "CommandPlayerSmsSpam.hpp"
#endif

#include "CommandPlayerWaypoint.hpp"
#include "CommandPlayerCopyWaypoint.hpp"

#include "CommandPlayerSpectateNinja.hpp"
#include "CommandPlayerSpectateLegit.hpp"

#include "CommandPlayerShowcam.hpp"
#include "CommandPlayerProfile.hpp"
#include "CommandPlayerBefriend.hpp"
#include "CommandPlayerInhistory.hpp"
#include "CommandPlayerCopyOutfit.hpp"
#include "CommandPlayerCopyVehicle.hpp"
#include "CommandPlayerAs.hpp"
#include "CommandPlayerAsAll.hpp"
#include "CommandPlayerCeoJoin.hpp"
#include "CommandPlayerIncrementStatNeutral.hpp"
#include "CommandPlayerIncrementStatAggressive.hpp"

#include "CommandPlayerKickSmart.hpp"
#include "CommandPlayerKickBreakup.hpp"
#if HAVE_BREAKUP_KICK
#include "CommandPlayerKickConfusion.hpp"
#endif
#include "CommandPlayerKickPoolsClosed.hpp"
#include "CommandPlayerKickNonHost.hpp"
#include "CommandPlayerKickOrgasm.hpp"
#include "CommandPlayerKickLoveLetter.hpp"
#include "CommandPlayerKickBlacklist.hpp"
#include "CommandPlayerKickHost.hpp"
#include "CommandPlayerKickHostType.hpp"
#include "CommandPlayerKickPickup.hpp"

#include "CommandPlayerCrashElegant.hpp"
#ifdef STAND_DEBUG
#include "CommandPlayerCrashTask.hpp"
#endif
#include "CommandPlayerCrashFootlettuce.hpp"
#include "CommandPlayerCrashVehicle.hpp"
#include "CommandPlayerCrashTrain.hpp"

#include "CommandPlayerTimeout.hpp"
#include "CommandPlayerFlowEventReaction.hpp"

namespace Stand
{
	CommandListPlayer::CommandListPlayer(CommandList* const parent, const PlayerProvider* pp, Label&& menu_name, std::vector<CommandName>&& command_names, commandflags_t flags)
		: CommandListConcealer(parent, std::move(menu_name), std::move(command_names), NOLABEL, flags, COMMAND_LIST_PLAYER), pp(pp)
	{
	}

	void CommandListPlayer::populate(compactplayer_t p)
	{
#if FREE_PROTECTIONS
		if (!g_gui.isRootStateFull())
		{
			this->createChild<CommandPlayerTimeout>();
			return;
		}
#endif

		const commandflags_t sublist_flags = getSublistFlags();

		// Exclusions
		if (!this->pp->single)
		{
			auto excludes = this->createChild<CommandList>(LOC("XCLDS"));

			PlayerProvider::excludes.populateList(excludes, {}, PlayerExcludes::SHOW_MODDER | PlayerExcludes::SHOW_ORG_MEMBERS | PlayerExcludes::SHOW_LIKELY_MODDER);
		}

		if (this->pp->single)
		{
			this->createChild<CommandPlayerInfo>();
			this->createChild<CommandPlayerClassifier>();
		}

		// Teleport
		{
			auto teleport = this->createChild<CommandList>(LOC("TP"), {}, NOLABEL, sublist_flags);

			if (this->pp->single)
			{
				teleport->createChild<CommandDivider>(LOC("PLYTP"));
				teleport->createChild<CommandPlayerTeleport>();
				teleport->createChild<CommandPlayerTeleportIntoVeh>();
				teleport->createChild<CommandPlayerVisit>();
			}

			teleport->createChild<CommandDivider>(LOC("PLYSMMN_D"));
			teleport->createChild<CommandPlayerTpToMe>();
			if (this->pp->single)
			{
				teleport->createChild<CommandPlayerTpToPlayer>();
			}
			teleport->createChild<CommandPlayerTpToMyWp>();
			teleport->createChild<CommandPlayerTpToMyObjective>();
			teleport->createChild<CommandPlayerTpToTheirWp>();
			teleport->createChild<CommandPlayerAptList>(sublist_flags);
			teleport->createChild<CommandPlayerCasinoTp>();
		}

		// Friendly
		{
			auto friendly = this->createChild<CommandList>(LOC("FRNDLY"), {}, NOLABEL, sublist_flags);

			// Vehicle
			{
				auto frienveh = friendly->createChild<CommandList>(LOC("VEH"));

				frienveh->createChild<CommandPlayerVehicleUpgrade>();
				frienveh->createChild<CommandPlayerVehRepair>();
				frienveh->createChild<CommandPlayerVehicleGodmode>();
				frienveh->createChild<CommandPlayerVehicleNoLockon>();
				frienveh->createChild<CommandPlayerVehEnginePowerMultiplier>();
			}

			// Give Collectibles
			{
				auto gc = friendly->createChild<CommandList>(LOC("GIVECOLL"));

				gc->createChild<CommandPlayerGiveCollectibles>(-1, LOC("ALL"));
				gc->createChild<CommandPlayerGiveCollectibles>(0, LOC("GIVECOLL_0"));
				gc->createChild<CommandPlayerGiveCollectibles>(1, LOC("GIVECOLL_1"));
				gc->createChild<CommandPlayerGiveCollectibles>(2, LOC("GIVECOLL_2"));
				gc->createChild<CommandPlayerGiveCollectibles>(3, LOC("GIVECOLL_3"));
				gc->createChild<CommandPlayerGiveCollectibles>(4, LOC("GIVECOLL_4"));
				gc->createChild<CommandPlayerGiveCollectibles>(5, LOC("GIVECOLL_5"));
				gc->createChild<CommandPlayerGiveCollectibles>(6, LOC("GIVECOLL_6"));
				gc->createChild<CommandPlayerGiveCollectibles>(8, LOC("GIVECOLL_8"));
				gc->createChild<CommandPlayerGiveCollectibles>(9, LOC("GIVECOLL_9"));
				gc->createChild<CommandPlayerGiveCollectibles>(10, LOC("GIVECOLL_10"));
				gc->createChild<CommandPlayerGiveCollectibles>(16, LOC("GIVECOLL_16"));
				gc->createChild<CommandPlayerGiveCollectibles>(17, LOC("GIVECOLL_17"));
				gc->createChild<CommandPlayerGiveCollectibles>(19, LOC("GIVECOLL_19"));
			}

#if REMOTE_STAT_WRITING
			// Fast Run Ability
			{
				auto fast_run = friendly->createChild<CommandList>(LOC("FSTRUN"));
				fast_run->createChild<CommandPlayerObtainFastRun>();
				fast_run->createChild<CommandPlayerRelinquishFastRun>();
			}
#endif

			if (this->pp->single)
			{
				//friendly->createChild<CommandPlayerGift>();
			}
#if REMOTE_STAT_WRITING
			friendly->createChild<CommandPlayerSetRank>();
#endif
			friendly->createChild<CommandPlayerCeopay>();
			friendly->createChild<CommandPlayerRp>();
			friendly->createChild<CommandPlayerDropFigurines>();
			friendly->createChild<CommandPlayerDropCards>();
			friendly->createChild<CommandPlayerAutoheal>();
			friendly->createChild<CommandPlayerBail>();
			friendly->createChild<CommandPlayerOtr>();
			friendly->createChild<CommandPlayerSnack>();
			friendly->createChild<CommandPlayerExplosiveHits>();
			friendly->createChild<CommandPlayerImpactParticles>();
			if (this->pp->single)
			{
				friendly->createChild<CommandPlayerGiveSh>();
			}
		}

		// Chat
		{
			auto chat = this->createChild<CommandList>(LOC("CHT"), {}, NOLABEL, sublist_flags);

			chat->createChild<CommandPlayerChatMock>();
			chat->createChild<CommandPlayerChatOwoify>();
			if (this->pp->single)
			{
				chat->createChild<CommandPlayerSendPM>();
			}
		}

		// Weapons
		{
			auto weapons = this->createChild<CommandList>(LOC("WPN"), {}, NOLABEL, sublist_flags);

			weapons->createChild<CommandPlayerArm>();
			weapons->createChild<CommandPlayerAmmo>();
			weapons->createChild<CommandPlayerParachute>();
			weapons->createChild<CommandPlayerDisarm>();
		}

		// Trolling
		{
			auto trolling = this->createChild<CommandList>(LOC("TROLL"), {}, NOLABEL, sublist_flags);

			// Vehicle
			{
				auto vehicle = trolling->createChild<CommandList>(LOC("VEH"), {}, NOLABEL, sublist_flags);

				if (this->pp->single)
				{
					vehicle->createChild<CommandPlayerVehKill>();
					vehicle->createChild<CommandPlayerVehFlip>();
					vehicle->createChild<CommandPlayerVehSpin>();
					vehicle->createChild<CommandPlayerVehEMP>();
					vehicle->createChild<CommandPlayerVehIgnite>();
					vehicle->createChild<CommandPlayerVehDelete>();
					vehicle->createChild<CommandPlayerVehPopTyres>();
					vehicle->createChild<CommandPlayerVehSlingshot>();
					vehicle->createChild<CommandPlayerVehLockDoors>();
					vehicle->createChild<CommandPlayerVehDowngrade>();
					vehicle->createChild<CommandPlayerVehTurnAround>();
					vehicle->createChild<CommandPlayerVehDetachWing>();
					vehicle->createChild<CommandPlayerVehDetachWheel>();
				}

				vehicle->createChild<CommandPlayerVehSlipperyTyres>();

				if (this->pp->single)
				{
					vehicle->createChild<CommandPlayerVehRemoveDoors>();
					vehicle->createChild<CommandPlayerVehDestroyProp>();
					vehicle->createChild<CommandPlayerVehBreakTailBoom>();
				}
			}

			if (this->pp->single)
			{
				trolling->createChild<CommandPlayerAttackers>(sublist_flags);

				// Ram
				{
					auto ram = trolling->createChild<CommandList>(LOC("RAM"));
					auto action = ram->createChild<CommandPlayerRam>();
					action->speed = ram->createChild<CommandPlayerRamSpeed>();
					action->model = ram->createChild<CommandListSelectVehicleModel>(LOC("VEH"), {}, NOLABEL, CommandListSelectVehicleModel::RANDOM, true);
					ram->createChild<CommandTogglePointer>(&action->invisible, LOC("RAM_INVIS"));
				}
			}

			trolling->createChild<CommandPlayerSetWantedLevel>();
			trolling->createChild<CommandPlayerFreeze>();
			if (this->pp->single)
			{
				trolling->createChild<CommandPlayerFakeMoneyDrop>();
				trolling->createChild<CommandPlayerBlockPassiveMode>();
			}
			trolling->createChild<CommandPlayerForceCamFront>();
			//trolling->createChild<CommandPlayerTransactionError>();
			trolling->createChild<CommandPlayerRagdoll>();
			if (this->pp->single)
			{
				trolling->createChild<CommandPlayerSoundSpam>();
			}
			trolling->createChild<CommandPlayerShakeCam>();
			if (this->pp->single)
			{
				trolling->createChild<CommandGravitateNPCs>();
				trolling->createChild<CommandAggressiveNPCs>();
				trolling->createChild<CommandPlayerMuggerLoop>();
				trolling->createChild<CommandPlayerBeast>();
			}
			trolling->createChild<CommandPlayerKill>();
			trolling->createChild<CommandPlayerCage>();
			trolling->createChild<CommandPlayerExplode>();

			if (this->pp->single)
			{
				//trolling->createChild<CommandPlayerBlame>();

				// Particle Spam
				{
					auto particle = trolling->createChild<CommandList>(LOC("PLRPTFXSPM"));
					auto particles = particle->makeChild<CommandListSelectParticle>(LOC("PARTICLE"));
					auto scale = particle->makeChild<CommandPlayerParticleSpamScale>();
					auto delay = particle->makeChild<CommandPlayerParticleSpamDelay>();
					auto hide = particle->makeChild<CommandToggle>(LOC("HDELCL"), {}, LOC("HDELCL_H"), true);
					particle->createChild<CommandPlayerParticleSpam>(particles.get(), delay.get(), scale.get(), hide.get());
					particle->children.emplace_back(std::move(particles));
					particle->children.emplace_back(std::move(scale));
					particle->children.emplace_back(std::move(delay));
					particle->children.emplace_back(std::move(hide));
				}

				trolling->createChild<CommandPlayerBounty>(nullptr);
			}
			else
			{
				auto bounty_self = trolling->makeChild<CommandToggle>(LOC("BNTYSLF"), {}, LOC("BNTYSLF_H"));
				trolling->createChild<CommandPlayerBounty>(bounty_self.get());
				trolling->children.emplace_back(std::move(bounty_self));
			}

			// Loop Bounty
			{
				auto loop = trolling->createChild<CommandList>(LOC("LOOPBNTY"));
				auto val = loop->createChild<CommandPlayerSlider>(LOC("C_V"), CMDNAMES_OBF("loopbountyvalue"), NOLABEL, 1000, 10000, 10000, 1000);
				auto cmd = loop->createChild<CommandPlayerLoopBounty>(val);
				if (!this->pp->single)
				{
					cmd->self = loop->createChild<CommandToggle>(LOC("BNTYSLF"), {}, LOC("BNTYSLF_H"));
				}
			}

			// Send Notifications
			{
				auto notify = trolling->createChild<CommandList>(LOC("PLY_NOTIFY"), {}, NOLABEL, sublist_flags);

				notify->createChild<CommandPlayerNotifyRemoved>();
				notify->createChild<CommandPlayerNotifyStolen>();
				notify->createChild<CommandPlayerNotifyBanked>();
				notify->createChild<CommandPlayerNotifySpam>();
			}

			// Send To Job
			{
				auto sendtojob = trolling->createChild<CommandList>(LOC("SND2JB"), {}, NOLABEL, sublist_flags);

				sendtojob->createChild<CommandPlayerSendtojob>(0, LOC("SND2JB_0"), { CMDNAME("sendtojob") });
				sendtojob->createChild<CommandPlayerSendtojob>(1, LOC("SND2JB_1"));
				sendtojob->createChild<CommandPlayerSendtojob>(2, LOC("SND2JB_2"));
				sendtojob->createChild<CommandPlayerSendtojob>(3, LOC("SND2JB_3"));
				sendtojob->createChild<CommandPlayerSendtojob>(4, LOC("SND2JB_4"));
				sendtojob->createChild<CommandPlayerSendtojob>(5, LOC("SND2JB_5"));
				sendtojob->createChild<CommandPlayerSendtojob>(6, LOC("SND2JB_6"));
				sendtojob->createChild<CommandPlayerSendtojob>(7, LOC("SND2JB_7"));
			}

			trolling->createChild<CommandPlayerVehKick>();
			trolling->createChild<CommandPlayerInteriorKick>();
			trolling->createChild<CommandPlayerDisableDrivingVehicles>();
			if (this->pp->single)
			{
				trolling->createChild<CommandPlayerCeoKick>();
			}
			trolling->createChild<CommandPlayerInfiniteLoading>();
			trolling->createChild<CommandPlayerInfiniteRing>();
			trolling->createChild<CommandPlayerForceFmMission>();
			trolling->createChild<CommandPlayerTriggerDefend>();
		}

#if SMS_OPTIONS
		// Send Text Message
		{
			auto sms = this->createChild<CommandList>(LOC("SMS"), {}, NOLABEL, sublist_flags);

			auto text = sms->createChild<CommandPlayerSmsText>();

			auto sender = sms->createChild<CommandPlayerSmsSend>(text);
			sms->createChild<CommandPlayerSmsSpam>(sender);
		}
#endif

		if (this->pp->single)
		{
			this->createChild<CommandPlayerWaypoint>();
			this->createChild<CommandPlayerCopyWaypoint>();

			// Spectate
			{
				auto spectate = this->createChild<CommandList>(LOC("PLYSPEC"));
		
				spectate->createChild<CommandPlayerSpectateNinja>();
				spectate->createChild<CommandPlayerSpectateLegit>();
			}
		}

		this->createChild<CommandPlayerShowcam>();

		if (this->pp->single)
		{
			this->createChild<CommandPlayerProfile>();
			this->createChild<CommandPlayerBefriend>();
			this->createChild<CommandPlayerInhistory>();
			this->createChild<CommandPlayerCopyOutfit>();
			this->createChild<CommandPlayerCopyVehicle>();
			this->createChild<CommandPlayerAs>();
			this->createChild<CommandPlayerCeoJoin>();
			((CommandPlayer*)this)->pong_command = createChild<CommandPlayerPong>();
		}
		else
		{
			this->createChild<CommandPlayerAsAll>();
		}

		// Increment Commend/Report Stats
		{
			auto ise = this->createChild<CommandList>(LOC("PLYISE"), {}, NOLABEL, sublist_flags);

			ise->createChild<CommandDivider>(LOC("CMEND"));
			ise->createChild<CommandPlayerIncrementStatNeutral>(LOC("ISE_H"), CMDNAMES_OBF("commendhelpful"), STAT_HELPFUL);
			ise->createChild<CommandPlayerIncrementStatNeutral>(LOC("ISE_F"), CMDNAMES_OBF("commendfriendly"), STAT_FRIENDLY);
			ise->createChild<CommandDivider>(LOC("RPRPT"));
			ise->createChild<CommandPlayerIncrementStatAggressive>(LOC("ISE_G2"), CMDNAMES_OBF("reportgriefing"), STAT_GRIEFING);
			ise->createChild<CommandPlayerIncrementStatAggressive>(LOC("ISE_X2"), CMDNAMES_OBF("reportexploits"), STAT_EXPLOITS);
			ise->createChild<CommandPlayerIncrementStatAggressive>(LOC("ISE_B2"), CMDNAMES_OBF("reportbugabuse"), STAT_GAME_EXPLOITS);
			ise->createChild<CommandPlayerIncrementStatAggressive>(LOC("ISE_TC_A"), CMDNAMES_OBF("reportannoying"), STAT_TC_ANNOYINGME);
			ise->createChild<CommandPlayerIncrementStatAggressive>(LOC("ISE_TC_H"), CMDNAMES_OBF("reporthate"), STAT_TC_HATE);
			ise->createChild<CommandPlayerIncrementStatAggressive>(LOC("ISE_VC_A"), CMDNAMES_OBF("reportvcannoying"), STAT_VC_ANNOYINGME);
			ise->createChild<CommandPlayerIncrementStatAggressive>(LOC("ISE_VC_H"), CMDNAMES_OBF("reportvchate"), STAT_VC_HATE);
		}

		{
			auto kick = this->createChild<CommandList>(LOC("PLYKCK"), {}, NOLABEL, sublist_flags);

			kick->createChild<CommandPlayerKickSmart>();
#if HAVE_BREAKUP_KICK
			kick->createChild<CommandPlayerKickBreakup>(LOC("PLYKCK_B"), CMDNAMES_OBF("breakup"), LOC("PLYKCK_H_DSCRT"), -1);
#endif
#if ENABLE_BAN_KICK
	#if HAVE_BREAKUP_KICK
			kick->createChild<CommandPlayerKickBreakup>(LOC("PLYBAN"), CMDNAMES_OBF("ban"), LOC("PLYKCK_H_DSCRT"), /* BAIL_CONSOLE_BAN */ 19);
	#else
			kick->createChild<CommandPlayerKickBreakup>(LOC("PLYBAN"), CMDNAMES_OBF("ban"), LOC("PLYKCK_H_HO"), /* BAIL_CONSOLE_BAN */ 19);
	#endif
#endif
#if HAVE_BREAKUP_KICK
			if (this->pp->single)
			{
				kick->createChild<CommandPlayerKickConfusion>();
			}
#endif
			kick->createChild<CommandPlayerKickPoolsClosed>();
			if (this->pp->single)
			{
				kick->createChild<CommandPlayerKickLoveLetter>();
				kick->createChild<CommandPlayerKickOrgasm>();
			}
			kick->createChild<CommandPlayerKickBlacklist>();
			kick->createChild<CommandPlayerKickHost>();
			if (g_tunables.getBool(ATSTRINGHASH("player_commands.bekick")))
			{
				kick->createChild<CommandPlayerKickHostType>(LOC("BE_K"), CMDNAMES("bekick"), LOC("PLYKCK_H_HO"), AbstractPlayer::BATTLEYE_KICK);
			}
			if (g_tunables.getBool(ATSTRINGHASH("player_commands.beban")))
			{
				kick->createChild<CommandPlayerKickHostType>(LOC("BE_B"), CMDNAMES("beban"), Label::combineWithSpace(LOC("BE_B_H"), LOC("PLYKCK_H_HO")), AbstractPlayer::BATTLEYE_BAN);
			}
			kick->createChild<CommandPlayerKickNonHost>();
			if (this->pp->single)
			{
				kick->createChild<CommandPlayerKickPickup>();
			}
		}

		{
			auto crash = this->createChild<CommandList>(LOC("PLYCRSH"), {}, NOLABEL, sublist_flags);

			crash->createChild<CommandPlayerCrashElegant>();
			if (this->pp->single)
			{
#ifdef STAND_DEBUG
				crash->createChild<CommandPlayerCrashTask>();
#endif
				crash->createChild<CommandPlayerCrashFootlettuce>();
				crash->createChild<CommandPlayerCrashVehicle>();
				crash->createChild<CommandPlayerCrashTrain>();
			}
		}

		// Network Traffic
		if (this->pp->single)
		{
			this->createChild<CommandPlayerTimeout>();

			{
				auto netreactions = this->createChild<CommandList>(LOC("PTX_NET"), {}, NOLABEL, sublist_flags);

				netreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_net_event_reactions[p], REACTION_BLOCK, LOC("PTX_NET_R_B"), CMDNAMES_OBF("ignore"));
				netreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_net_event_reactions[p], REACTION_LOG_FILE, LOC("RCT_L_F"), CMDNAMES_OBF("netlog"));
				netreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_net_event_reactions[p], REACTION_LOG_CONSOLE, LOC("RCT_L_C"));
				netreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_net_event_reactions[p], REACTION_TOAST, LOC("RCT_T"));
			}

			{
				auto syncinreactions = this->createChild<CommandList>(LOC("PTX_SI"), {}, NOLABEL, sublist_flags);

				syncinreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_sync_in_reactions[p], REACTION_BLOCK, LOC("PTX_NET_R_B"), CMDNAMES_OBF("blocksync"));
				syncinreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_sync_in_reactions[p], REACTION_LOG_FILE, LOC("RCT_L_F"), CMDNAMES_OBF("syncinlog"));
				syncinreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_sync_in_reactions[p], REACTION_LOG_CONSOLE, LOC("RCT_L_C"));
				syncinreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_sync_in_reactions[p], REACTION_TOAST, LOC("RCT_T"));
			}
		}

		{
			auto syncoutreactions = this->createChild<CommandList>(LOC("PTX_SO"), {}, NOLABEL, sublist_flags);

			syncoutreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_sync_out_reactions[p], REACTION_BLOCK, LOC("PTX_NET_R_B"), CMDNAMES_OBF("desync"), this->pp->single ? LOC("PLY_BOS_H_1") : LOC("PLY_BOS_H_A"));
			syncoutreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_sync_out_reactions[p], REACTION_LOG_FILE, LOC("RCT_L_F"), CMDNAMES_OBF("syncoutlog"));
			syncoutreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_sync_out_reactions[p], REACTION_LOG_CONSOLE, LOC("RCT_L_C"));
			syncoutreactions->createChild<CommandPlayerFlowEventReaction>(&g_hooking.player_sync_out_reactions[p], REACTION_TOAST, LOC("RCT_T"));
		}
	}

	commandflags_t CommandListPlayer::getSublistFlags() const
	{
		if (this->pp->single)
		{
			return CMDFLAGS_LIST | CMDFLAG_TEMPORARY;
		}
		return CMDFLAGS_LIST;
	}

	void CommandListPlayer::registerCommand(CommandPhysical* const cmd) const
	{
		if (isSingle())
		{
			cmd->flags |= CMDFLAG_TEMPORARY;
		}
		registerCommandNames(cmd);
	}

	void CommandListPlayer::registerCommandNames(CommandPhysical* const cmd) const
	{
		const CommandName suffix = getCommandNamesSuffix();
		for (auto& command_name : cmd->command_names)
		{
			CommandPhysical::collapse_command_names.emplace(cmdNameToUtf16(command_name));
			command_name.append(suffix);
		}
		if (cmd->isListAction())
		{
			cmd->as<CommandListAction>()->updateCommandNamesOfChildren();
		}
	}

	bool CommandListPlayer::isSingle() const noexcept
	{
		return pp->single;
	}

	void CommandListPlayer::ensureOnlineAndWarnSelfharm(Command* command_for_warning, Click& click, std::function<void(ThreadContext)>&& callback) const
	{
		if(click.inOnline())
		{
			if (!pp->isUser())
			{
				callback(click.thread_context);
				return;
			}
			command_for_warning->warnSelfHarm(click, [command_for_warning, callback{std::move(callback)}](ThreadContext thread_context)
			{
				if (command_for_warning->isT<CommandToggleNoCorrelation>())
				{
					command_for_warning->as<CommandToggleNoCorrelation>()->setOnIndication(true);
				}
				callback(thread_context);
			});
		}
		if (command_for_warning->isT<CommandToggleNoCorrelation>())
		{
			command_for_warning->as<CommandToggleNoCorrelation>()->setOnIndication(false);
		}
	}
}
