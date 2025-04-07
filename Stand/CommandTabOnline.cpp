#include "CommandTabOnline.hpp"

#include <soup/ObfusString.hpp>
#include <soup/rand.hpp>

#include "Camgr.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandListSelect.hpp"
#include "CommandReadonlyLink.hpp"
#include "CommandToggleBitflag.hpp"
#include "CommandTogglePointer.hpp"
#include "evtForceQuitting.hpp"
#include "Gui.hpp"
#include "PlayerHistory.hpp"
#include "RemoteGamer.hpp"

#include "CommandNewSession.hpp"

#include "CommandMmRegion.hpp"
#include "CommandMmPool.hpp"
#include "CommandMmMagnet.hpp"
#include "CommandMmOff.hpp"

#include "CommandSeamless.hpp"
#include "CommandSessionHopper.hpp"
#include "CommandTrnsSpdupBroadcast.hpp"
#include "CommandTrnsSpdupFmmc.hpp"
#include "CommandTrnsSpdupSpawn.hpp"
//#include "CommandThunderJoin.hpp"
#include "CommandJoinGroupOverride.hpp"
#include "CommandSkipSwoop.hpp"
#include "CommandTransitionHelper.hpp"
#include "CommandDisableSpawnActivity.hpp"
#include "CommandNoDlcIntro.hpp"
#include "CommandShowTransitionState.hpp"
#include "CommandShowArraySynchProgress.hpp"

#include "CommandKeepOnlineGlobalAtZero.hpp"
#include "CommandThousandsSep.hpp"
#include "CommandFullPlayerList.hpp"
#include "CommandFmBanner2Notify.hpp"
#include "CommandSellInPrivate.hpp"
#include "CommandNoLscBarriers.hpp"
#include "CommandOnlinePhoneAnims.hpp"
#include "CommandDisableMechanicCooldown.hpp"
#include "CommandNoWorkCooldown.hpp"
#include "CommandNoOrbCooldown.hpp"
#include "CommandDisableKosatkaMissileCooldown.hpp"
#include "CommandDisableKosatkaMissileRangeLimit.hpp"
#include "CommandDisableNanoDroneRangeLimit.hpp"
#include "CommandDisableNanoDroneAbilityCooldowns.hpp"
#include "CommandNoBusinessBattleMusic.hpp"
#include "CommandNoFmEventEndAudio.hpp"
#include "CommandTogglableBigmap.hpp"
#include "CommandPausable.hpp"
#include "CommandAnyVehRc.hpp"
#include "CommandGtaRcFreecam.hpp"
#include "CommandSpeedupDial.hpp"
#include "CommandDisableDeathbarriers.hpp"
#include "CommandBypassInteriorRestrictions.hpp"
#include "CommandPassiveWeaponizedVehicles.hpp"
#include "CommandBypassPiMenuGreyedOut.hpp"
#include "CommandUnlockCasino.hpp"
#include "CommandBypassTimeTrialUnsuitable.hpp"
#include "CommandDisableDripfeed.hpp"
#include "CommandDisableEmptyNotifications.hpp"
#include "CommandDisablePassiveCooldown.hpp"
#include "CommandCommendToast.hpp"
#include "CommandNoDailyExpenses.hpp"
#include "CommandGhostNearbyDrivers.hpp"

#include "CommandQuickProgress.hpp"
#include "CommandListProtections.hpp"

#include "CommandShowTyping.hpp"
#include "CommandTypingPosClosed.hpp"
#include "CommandTypingPosOpen.hpp"

#include "CommandSendChatMessage.hpp"

#include "CommandChatSpam.hpp"
#include "CommandChatSing.hpp"
#include "CommandHideTyping.hpp"
#include "CommandChatLogAuxfile.hpp"
#include "CommandChatLogMainfile.hpp"
#include "CommandChatLogConsole.hpp"
#include "CommandTabComplete.hpp"
#include "CommandPaste.hpp"
#include "CommandHtml.hpp"
#include "CommandUncensored.hpp"
#include "CommandChatAlwaysOpen.hpp"
#include "CommandOpenChatWhenScrolling.hpp"

#include "CommandSessionBrowser.hpp"

#include "CommandListSpoofing.hpp"

#include "CommandListSessionScripts.hpp"
#include "CommandSessionCode.hpp"
#include "CommandPlayerbar.hpp"

#include "LeaveNotifications.hpp"
#include "LeaveReasons.hpp"

#include "CommandBlockJoinsAnyone.hpp"
#include "CommandBlockJoinsStrangers.hpp"
#include "CommandBlockJoinsNonWhitelisted.hpp"
#include "CommandBlockJoinsAdvertisers.hpp"
#include "CommandBlockJoinsRemoved.hpp"
#include "CommandBjMessage.hpp"
#include "CommandRemovedPlayers.hpp"

#include "CommandStandUserIdentification.hpp"

#include "CommandCustomVoteKick.hpp"
//#include "CommandScriptHostLoop.hpp"
#include "CommandJoinDesynced.hpp"
#include "CommandSessionThunder.hpp"
#include "CommandScriptHost.hpp"
#include "CommandRestartfm.hpp"
#include "CommandRejoin.hpp"
#include "CommandBeAlone.hpp"
#include "CommandPlayerMagnet.hpp"
#include "CommandSessionType.hpp"
#include "CommandClearBlacklist.hpp"

#include "CommandRidTools.hpp"

#include "CommandCeoColourSlot.hpp"
#include "CommandInteractionMenuClick.hpp"
#include "CommandCeoName.hpp"
#include "CommandCeoType.hpp"
//#include "CommandCeoDisableCap.hpp"

#include "CommandListMental.hpp"

#include "CommandWithdraw.hpp"
#include "CommandWalletHopper.hpp"

#include "CommandDisableInsurancePay.hpp"
#include "CommandHalloween.hpp"
#include "CommandPeyotes.hpp"

#include "CommandListAssetHashes.hpp"

#include "CommandActionOnlineGlobal.hpp"
#include "CommandHeliTaxi.hpp"
#include "CommandRequestNanoDrone.hpp"

#include "CommandOtr.hpp"
#include "CommandBst.hpp"
#include "CommandBstOnce.hpp"
#include "CommandRemoveBounty.hpp"
#include "CommandReportStats.hpp"

#include "CommandChatCommandsSelf.hpp"
#include "CommandChatCommandsFriends.hpp"
#include "CommandChatCommandsCrew.hpp"
#include "CommandChatCommandsTeam.hpp"
#include "CommandChatCommandsStrangers.hpp"

#include "CommandFlowEventGlobal.hpp"
#include "CommandRevealOtr.hpp"
#include "CommandRevealInvisible.hpp"
#include "CommandBecomeOrbitalCannon.hpp"
#include "CommandAimMode.hpp"
#include "CommandAdminDlc.hpp"
#include "CommandBlipTags.hpp"

namespace Stand
{
	void addChatCommandsPermission(CommandList* group, CommandPerm* addr, CommandPerm perm)
	{
		group->createChild<CommandToggleBitflag<CommandPerm>>(getCommandPermLabel(perm), addr, perm);
	}

	void addChatCommandsPermissions(CommandList* group, CommandPerm* addr)
	{
		group->createChild<CommandDivider>(LOC("PERMS"));
		addChatCommandsPermission(group, addr, COMMANDPERM_FRIENDLY);
		addChatCommandsPermission(group, addr, COMMANDPERM_NEUTRAL);
		addChatCommandsPermission(group, addr, COMMANDPERM_SPAWN);
		addChatCommandsPermission(group, addr, COMMANDPERM_RUDE);
		addChatCommandsPermission(group, addr, COMMANDPERM_AGGRESSIVE);
		addChatCommandsPermission(group, addr, COMMANDPERM_TOXIC);
	}

	CommandTabOnline::CommandTabOnline(CommandList* const parent)
		: CommandTab(parent, TAB_ONLINE, LOC("Online"), { CMDNAME("tonline"), CMDNAME("online") })
	{
	}

	void CommandTabOnline::populate()
	{
		if (g_gui.isRootStateFull())
		{
			// New Session
			this->createChild<CommandNewSession>();

			// Transitions
			{
				auto* const transitions = this->createChild<CommandList>(LOC("TRNSS"));

				// Matchmaking
				{
					auto mm = transitions->createChild<CommandList>(LOC("MM"));

					mm->createChild<CommandMmRegion>();
					mm->createChild<CommandListAssetHashes>();

					// Pool Override
					{
						auto pool = mm->createChild<CommandList>(LOC("MMPOOLOVRD"));

						pool->createChild<CommandTogglePointer>(&g_hooking.spoof_matchmaking_pool, LOC("MMPOOLOVRD"), { CMDNAME("mmpooltoggle") });
						pool->createChild<CommandMmPool>();
					}

					mm->createChild<CommandMmMagnet>();
					mm->createChild<CommandMmOff>();
				}

				// Seamless Session Switching
				{
					auto seamless = transitions->createChild<CommandList>(LOC("SSS"));

					seamless->createChild<CommandSeamless>();
					CommandSeamless::world_state_config = seamless->createChild<CommandListSelect>(LOC("WRLDSTCNG"), {}, NOLABEL, std::vector<CommandListActionItemData>{
						{0, LOC("DOFF")},
						{ 1, LOC("WRLDSTCNG_1") },
						{ 2, LOC("WRLDSTCNG_2") },
						{ 3, LOC("WRLDSTCNG_3") },
					}, 1);
				}

				// Speed Up
				{
					auto speedup = transitions->createChild<CommandList>(LOC("SPDUP"));

					speedup->createChild<CommandTrnsSpdupBroadcast>();
					speedup->createChild<CommandTrnsSpdupFmmc>();
					speedup->createChild<CommandTrnsSpdupSpawn>();
					//speedup->createChild<CommandThunderJoin>();
				}

				// Session Hopper
				{
					auto session_hopper = transitions->createChild<CommandList>(LOC("SESSHOP"));

					auto min_players = session_hopper->makeChild<CommandSlider>(LOC("MINPLYS"), { CMDNAME("hopminplayers") }, NOLABEL, 1, 30, 1);
					session_hopper->createChild<CommandSessionHopper>(min_players.get());
					session_hopper->children.emplace_back(std::move(min_players));
				}

				transitions->createChild<CommandLambdaAction>(LOC("UNSTK"), { CMDNAME("unstuck") }, NOLABEL, [](Click& click)
				{
					click.ensureYieldableScriptThread([]
					{
						*ScriptGlobal(GLOBAL_NEW_SESSION).as<int*>() = 0;
						ScriptGlobal(GLOBAL_TRANSITION_STATE).set<int>(TRANSITION_STATE_TERMINATE_MAINTRANSITION);
						STREAMING::SWITCH_TO_MULTI_SECONDPART(g_player_ped);

						Script::current()->yield(200);

						TransitionHelper::doTransition(INVITE_ONLY);
					});
				});

				transitions->createChild<CommandJoinGroupOverride>();
				transitions->createChild<CommandSkipSwoop>();
				transitions->createChild<CommandTogglePointer>(&g_gui.show_status_message, LOC("SHWRSMSG"), CMDNAMES("supportmessage"));
				transitions->createChild<CommandDisableSpawnActivity>();
				transitions->createChild<CommandNoDlcIntro>();
				transitions->createChild<CommandTransitionHelper>();
				transitions->createChild<CommandShowTransitionState>();
				transitions->createChild<CommandShowArraySynchProgress>();
			}

			// Session
			{
				auto session = this->createChild<CommandList>(LOC("SESS"));

				session->createChild<CommandListSessionScripts>();

				session->createChild<CommandSessionCode>();

				// Block Joins
				{
					auto bjs = session->createChild<CommandList>(LOC("BJS"));

					bjs->createChild<CommandBlockJoinsAnyone>();
					bjs->createChild<CommandBlockJoinsStrangers>();
					bjs->createChild<CommandBlockJoinsNonWhitelisted>();
					bjs->createChild<CommandBlockJoinsAdvertisers>();
					bjs->createChild<CommandBlockJoinsRemoved>();
					bjs->createChild<CommandDivider>(LOC("META"));
					bjs->createChild<CommandBjMessage>();
					bjs->createChild<CommandRemovedPlayers>();
					
					// Notifications
					{
						auto notifs = bjs->createChild<CommandList>(LOC("NOTFS"));

						// From Anyone
						{
							auto fa = notifs->createChild<CommandList>(LOC("BJ_ALL"));
							g_hooking.block_joins_from_anyone_notify.populateList(fa);
						}

						// From Strangers
						{
							auto fa = notifs->createChild<CommandList>(LOC("BJ_STR"));
							g_hooking.block_joins_from_strangers_notify.populateList(fa);
						}

						// From Non-Whitelisted
						{
							auto nw = notifs->createChild<CommandList>(LOC("BJ_WHT"));
							g_hooking.block_joins_from_non_whitelisted_notify.populateList(nw);
						}

						// From Known Advertisers
						{
							auto ad = notifs->createChild<CommandList>(LOC("BJ_ADS"));
							g_hooking.block_joins_from_known_advertisers_notify.populateList(ad);
						}

						// From Kicked Players
						{
							auto kp = notifs->createChild<CommandList>(LOC("BJ_REM"));
							g_hooking.block_joins_from_removed_players_notify.populateList(kp);
						}

						// Player Join Reactions
						{
							auto pjr = notifs->createChild<CommandList>(LOC("JOINR"));
							g_hooking.block_joins_player_join_reactions_notify.populateList(pjr);
						}
					}
				}

				session->createChild<CommandPlayerbar>();

				{
					auto leave_reasons = session->createChild<CommandList>(LOC("LR"), {}, LOC("LR_H"));

					leave_reasons->createChild<CommandDivider>(LOC("LR_WHERE"));
					LeaveReasons::toast_config.populateList(leave_reasons, CMDNAMES_OBF("lr"));
					leave_reasons->createChild<CommandDivider>(LOC("LR_WHAT"));
					leave_reasons->createChild<CommandTogglePointer>(&LeaveReasons::normal_enabled, LOC("LR_SN"), CMDNAMES_OBF("lrnormal"));
					leave_reasons->createChild<CommandTogglePointer>(&LeaveReasons::closed_game_enabled, LOC("LR_SQ"), CMDNAMES_OBF("lrclose"));
					leave_reasons->createChild<CommandTogglePointer>(&LeaveReasons::blocked_from_joining_enabled, LOC("LR_SB"), CMDNAMES_OBF("lrblocked"));
				}
				{
					auto leave_notifs = session->createChild<CommandList>(LOC("LEAVET"));

					LeaveNotifications::toast_config.populateList(leave_notifs, CMDNAMES_OBF("leave"));
				}

				// Stand User Identification
				{
					auto sui = session->createChild<CommandList>(LOC("MDDRDT_SUI"));
					sui->createChild<CommandStandUserIdentification>();

					sui->createChild<CommandDivider>(LOC("HS"));
					sui->createChild<CommandTogglePointer>(&CommandPlayer::hostshare_enabled, LOC("HS"), {}, LOC("HS_H"));
					sui->createChild<CommandTogglePointer>(&CommandPlayer::hostshare_friends, LOC("4F"));
					sui->createChild<CommandTogglePointer>(&CommandPlayer::hostshare_crew, LOC("4C"));
					sui->createChild<CommandTogglePointer>(&CommandPlayer::hostshare_strangers, LOC("4S"));
				}

				session->createChild<CommandCustomVoteKick>();
				//session->createChild<CommandScriptHostLoop>();

				// Automatically Rejoin Desynced Sessions
				{
					auto list = session->createChild<CommandList>(LOC("AUTOREJOIN"));
					auto cmd = list->createChild<CommandJoinDesynced>();
					list->createChild<CommandDivider>(LOC("NOTFS"));
					cmd->toast_config.populateList(list);
				}

				session->createChild<CommandSessionThunder>();

				session->createChild<CommandScriptHost>();
				session->createChild<CommandRestartfm>();
				session->createChild<CommandRejoin>();
				session->createChild<CommandBeAlone>();

				session->createChild<CommandDivider>(LOC("HSTTLS"));
				session->createChild<CommandPlayerMagnet>();
				session->createChild<CommandSessionType>();
				session->createChild<CommandClearBlacklist>();
			}

			// Player History
			if (PlayerHistory::retained_player_history_command)
			{
				CommandPhysical::collapse_command_names.emplace(L"historynote");
				CommandPhysical::collapse_command_names.emplace(L"historytrack");
				CommandPhysical::collapse_command_names.emplace(L"redirect");
				CommandPhysical::collapse_command_names.emplace(L"historyprofile");
				CommandPhysical::collapse_command_names.emplace(L"historybefriend");
				CommandPhysical::collapse_command_names.emplace(L"historyjoin");
				CommandPhysical::collapse_command_names.emplace(L"historyspectate");
				CommandPhysical::collapse_command_names.emplace(L"historyinvite");
				CommandPhysical::collapse_command_names.emplace(L"historyblock");

				PlayerHistory::retained_player_history_command->setParent(this);
				this->emplaceVisible(std::move(PlayerHistory::retained_player_history_command));
				if (PlayerHistory::divider != nullptr)
				{
					PlayerHistory::updateDivider();
				}
			}

			// Session Browser
			{
				this->createChild<CommandSessionBrowser>();
			}

			// Enhancements
			{
				auto enhancements = this->createChild<CommandList>(LOC("ENHNCMTS"));

				enhancements->createChild<CommandLambdaActionScript>(LOC("RMLDR"), { CMDNAME("removeloader"), CMDNAME("removespinner"), CMDNAME("removetransactionpending") }, LOC("RMLDR_H"), [](const Click&)
				{
					HUD::BUSYSPINNER_OFF();
				});
				enhancements->createChild<CommandLambdaAction>(LOC("LVINT"), { CMDNAME("leaveinterior") }, NOLABEL, [](Click& click)
				{
					if (click.inOnline())
					{
						ScriptGlobal(GLOBAL_LEAVE_INTERIOR).set<BOOL>(TRUE);
					}
				});
				enhancements->createChild<CommandLambdaActionScript>(LOC("FRCSVE"), { CMDNAME("forcecloudsave"), CMDNAME("forcesave") }, NOLABEL, [](const Click&)
				{
					STATS::STAT_SAVE(0, 0, 3, 0);
				});
				enhancements->createChild<CommandKeepOnlineGlobalAtZero<GLOBAL_IDLE_TIMER_1, GLOBAL_IDLE_TIMER_2, GLOBAL_IDLE_TIMER_3, GLOBAL_IDLE_TIMER_4>>(LOC("NOAFK"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAMES("idlekick", "afkkick", "afkick")));
				enhancements->createChild<CommandThousandsSep>();
				enhancements->createChild<CommandFullPlayerList>();
				enhancements->createChild<CommandFmBanner2Notify>();
				enhancements->createChild<CommandSellInPrivate>();
				enhancements->createChild<CommandNoLscBarriers>();
				enhancements->createChild<CommandOnlinePhoneAnims>();
				enhancements->createChild<CommandSpeedupDial>();
				enhancements->createChild<CommandDisableDeathbarriers>();
				enhancements->createChild<CommandDisableMechanicCooldown>();
				enhancements->createChild<CommandKeepOnlineGlobalAtZero<GLOBAL_MKII_COOLDOWN>>(LOC("NOMKIICLDN"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAMES("mk2cooldown", "mkiicooldown")));
				enhancements->createChild<CommandNoWorkCooldown>();
				enhancements->createChild<CommandKeepOnlineGlobalAtZero<GLOBAL_EWO_COOLDOWN>>(LOC("NOEWOCLDN"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAME("ewocooldown")));
				enhancements->createChild<CommandDisablePassiveCooldown>();
				enhancements->createChild<CommandNoOrbCooldown>();
				enhancements->createChild<CommandDisableKosatkaMissileCooldown>();
				enhancements->createChild<CommandDisableKosatkaMissileRangeLimit>();
				enhancements->createChild<CommandDisableNanoDroneRangeLimit>();
				enhancements->createChild<CommandDisableNanoDroneAbilityCooldowns>();
				enhancements->createChild<CommandNoBusinessBattleMusic>();
				enhancements->createChild<CommandNoFmEventEndAudio>();
				enhancements->createChild<CommandTogglableBigmap>();
				enhancements->createChild<CommandPausable>();
				enhancements->createChild<CommandPassiveWeaponizedVehicles>();
				enhancements->createChild<CommandAnyVehRc>();
				enhancements->createChild<CommandGtaRcFreecam>();
				enhancements->createChild<CommandBypassInteriorRestrictions>();
				enhancements->createChild<CommandBypassPiMenuGreyedOut>();
				enhancements->createChild<CommandUnlockCasino>();
				enhancements->createChild<CommandBypassTimeTrialUnsuitable>();
				enhancements->createChild<CommandDisableDripfeed>();
				enhancements->createChild<CommandDisableEmptyNotifications>();
				enhancements->createChild<CommandCommendToast>();
				enhancements->createChild<CommandNoDailyExpenses>();
				enhancements->createChild<CommandTogglePointer>(&g_hooking.block_background_script, LOC("BBGS"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAMES("bgscript", "backgroundscript")), LOC("BBGS_H"));
				enhancements->createChild<CommandGhostNearbyDrivers>();
			}
		}

#if FREE_PROTECTIONS
		if (g_gui.root_state != GUI_MINIMAL)
#else
		if (g_gui.isRootStateFull())
#endif
		{
			// Protections
			this->createChild<CommandListProtections>();
		}

		if (g_gui.isRootStateFull())
		{
			// Quick Progress
			this->createChild<CommandQuickProgress>();
		}

#if !FREE_PROTECTIONS
		if (g_gui.isRootStateFull())
#endif
		{
			// Chat
			auto chat = this->createChild<CommandList>(LOC("CHT"));

			if (g_gui.isRootStateFull())
			{
				// Commands
				{
					auto chat_commands = chat->createChild<CommandList>(LOC("CMDS"), {}, LOC("CMDS_H"));

					chat_commands->createChild<CommandChatCommandsSelf>();
					// For Friends
					{
						auto* const group = chat_commands->createChild<CommandList>(LOC("4F"));

						group->createChild<CommandChatCommandsFriends>();
						addChatCommandsPermissions(group, &Click::perms_friends);
					}
					// For Crew Members
					{
						auto* const group = chat_commands->createChild<CommandList>(LOC("4C"));

						group->createChild<CommandChatCommandsCrew>();
						addChatCommandsPermissions(group, &Click::perms_crew);
					}
					// For Team Chat
					{
						auto* const group = chat_commands->createChild<CommandList>(LOC("4T"), {}, LOC("4T_H"));

						group->createChild<CommandChatCommandsTeam>();
						addChatCommandsPermissions(group, &Click::perms_team);
					}
					// For Strangers
					{
						auto* const group = chat_commands->createChild<CommandList>(LOC("4S"));

						group->createChild<CommandChatCommandsStrangers>();
						addChatCommandsPermissions(group, &Click::perms_strangers);
					}

					ChatCommandsCommon::prefix = chat_commands->createChild<CommandListSelect>(LOC("PFX"), {}, NOLABEL, std::vector<CommandListActionItemData>{
						{ 0, LIT(1, '!') },
						{ 1, LIT(1, '?') },
						{ 2, LIT(1, ';') },
						{ 3, LIT(1, ',') },
						{ 4, LIT(1, '.') },
						{ 5, LIT(1, '+') },
						{ 6, LIT(1, '-') },
						{ 7, LIT(1, '*') },
						{ 8, LIT(1, '/') },
						{ 9, LIT(1, '\\') },
						{ 10, LIT(1, '%') },
						{ 11, LIT(1, '=') },
						{ 12, LIT(1, '#') },
						{ 13, LIT(1, '$') },
						{ 14, LIT("€") },
					}, 0);
					ChatCommandsCommon::reply_prefix = chat_commands->createChild<CommandListSelect>(LOC("REPLPFX"), {}, NOLABEL, std::vector<CommandListActionItemData>{
						{ 0, LOC("NONE") },
						//{ 1, LIT(1, '>') },
						{ 2, LIT(1, '!') },
						{ 3, LIT(1, '?') },
						{ 4, LIT(1, ';') },
						{ 5, LIT(1, ',') },
						{ 6, LIT(1, '.') },
						{ 7, LIT(1, '+') },
						{ 8, LIT(1, '-') },
						{ 9, LIT(1, '*') },
						{ 10, LIT(1, '/') },
						{ 11, LIT(1, '\\') },
						{ 12, LIT(1, '%') },
						{ 13, LIT(1, '=') },
						{ 14, LIT(1, '#') },
						{ 15, LIT(1, '$') },
						{ 16, LIT("€") },
					}, 2);
					ChatCommandsCommon::reply_visibility = chat_commands->createChild<CommandListSelect>(LOC("REPLYVIS"), {}, NOLABEL, std::vector<CommandListActionItemData>{
						{ -1, LOC("EVERYONE") },
						{ (1 << 1) | (1 << 0), LOC("ISSUER_N_ME") },
						{ (1 << 1), LOC("ISSUER")},
						{ (1 << 0), LOC("PSC_M") },
						{ 0, LOC("NOONE") },
					}, -1);

					chat_commands->createChild<CommandReadonlyLink>(LOC("CMDBXDCS"), soup::ObfusString("https://commands.chat/"));
					chat_commands->createChild<CommandReadonlyLink>(LOC("CMDLST"), soup::ObfusString("https://stand.sh/commands"));
				}
			}

			// Reactions
			{
				auto reactions = chat->createChild<CommandList>(LOC("RCTS"));

				reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("CHT_ANY"), FlowEvent::CHAT_ANY);
				reactions->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::CHAT_AD);
				reactions->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::CHAT_PROFANITYBYPASS);
				reactions->createChildWithBuilder<CommandFlowEventGlobal>(FlowEvent::CHAT_SPAM);
			}

			if (g_gui.isRootStateFull())
			{
				// Log Chat Messages
				{
					auto chatlog = chat->createChild<CommandList>(LOC("CHTLG"), CMDNAMES_OBF("logchat", "chatlog"));

					chatlog->createChild<CommandChatLogAuxfile>();
					chatlog->createChild<CommandChatLogMainfile>();
					chatlog->createChild<CommandChatLogConsole>();
				}

				// Typing Indicator
				{
					auto typing = chat->createChild<CommandList>(LOC("TPNGIDCTR2"), {}, LOC("TPNGIDCTR_H"));

					typing->createChild<CommandShowTyping>();
					typing->createChild<CommandTypingPosClosed>();
					typing->createChild<CommandTypingPosOpen>();
				}

				// Send Message
				{
					auto send = chat->createChild<CommandList>(LOC("SNDMSG"));

					auto in_team_chat = send->makeChild<CommandToggle>(LOC("ITEAMCHT"), { CMDNAME("sayteam") });
					auto add_to_local_history = send->makeChild<CommandToggle>(LOC("LOCHIST"), { CMDNAME("saylocal") }, NOLABEL, true);
					send->createChild<CommandSendChatMessage>(in_team_chat.get(), add_to_local_history.get());
					send->children.emplace_back(std::move(add_to_local_history));
					send->children.emplace_back(std::move(in_team_chat));
				}

				// Spam
				{
					auto spammer = chat->createChild<CommandList>(LOC("SPAM"));

					auto message = spammer->makeChild<CommandInput>(LOC("TEXT"), { CMDNAME("spamtext") }, NOLABEL, "I have a small penis");
					auto interval_ms = spammer->makeChild<CommandSlider>(LOC("INTVAL_MS"), { CMDNAME("spaminterval") }, NOLABEL, 100, 60000, 5000, 1000);
					auto local = spammer->makeChild<CommandToggle>(LOC("LOCHIST"), { CMDNAME("spamlocal") }, LOC("LOCHIST_H_SPAM"), true);
					spammer->createChild<CommandChatSpam>(message.get(), interval_ms.get(), local.get());
					spammer->children.emplace_back(std::move(message));
					spammer->children.emplace_back(std::move(interval_ms));
					spammer->children.emplace_back(std::move(local));
				}

#if !FORCE_SEND_MESSAGES_BY_DEFAULT
				chat->createChild<CommandTogglePointer>(&g_hooking.force_send_chat, LOC("FRCSEND"), {}, LOC("FRCSEND_H"));
#endif
			}
#if FORCE_SEND_MESSAGES_BY_DEFAULT // allow free users to disable it
			chat->createChild<CommandTogglePointer>(&g_hooking.force_send_chat, LOC("FRCSEND"), {}, LOC("FRCSEND_H"));
#endif
			chat->createChild<CommandTogglePointer>(&g_hooking.force_receive_chat, LOC("FRCRECV"), {}, LOC("FRCRECV_H2"));
			if (g_gui.isRootStateFull())
			{
				chat->createChild<CommandTabComplete>();
				chat->createChild<CommandPaste>();
				chat->createChild<CommandUncensored>();
				chat->createChild<CommandHtml>();
				chat->createChild<CommandTogglePointer>(&g_gui.kill_loader_while_chatting, LOC("CHTRMLDR"), {}, LOC("CHTRMLDR_H"));
				chat->createChild<CommandOpenChatWhenScrolling>();
				chat->createChild<CommandChatAlwaysOpen>();
				chat->createChild<CommandHideTyping>();
				chat->createChild<CommandTogglePointer>(&g_hooking.chat_label_timestamps, LOC("TIMESTAMP_MS"), CMDNAMES_OBF("chattimestamp"));
				chat->createChild<CommandTogglePointer>(&g_hooking.owoify_my_outgoing_messages, LOC("OWOMYMSGS"), CMDNAMES_OBF("owoifymychats"), LOC("OWOMYMSGS_H"));
				chat->createChild<CommandChatSing>();
			}
		}

		if (g_gui.isRootStateFull() || g_gui.root_state == GUI_MINIMAL)
		{
			this->createChild<CommandListSpoofing>();
		}
		if (g_gui.isRootStateFull())
		{
			// Reactions
			{
				auto reactions = this->createChild<CommandList>(LOC("RCTS"));

				reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("JOINR"), FlowEvent::MISC_JOIN);
				reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("HOSTR"), FlowEvent::MISC_HOSTCHANGE);
				reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("SHR"), FlowEvent::MISC_SCRIPTHOSTCHANGE);
				{
					auto vote_kick_reactions = reactions->createChild<CommandList>(LOC("VKR"));
					vote_kick_reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("VKR_A"), FlowEvent::MISC_VOTEKICK_ANY);
					vote_kick_reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("VKR_M"), FlowEvent::MISC_VOTEKICK_ME);
				}
				reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("LLKR"), FlowEvent::MISC_LOVELETTER, LOC("GEN_H_HOST"));
				reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("BOUNTYR"), FlowEvent::MISC_BOUNTY, LOC("BOUNTYR_H"));
				reactions->createChildWithBuilder<CommandFlowEventGlobal>(LOC("PTFXSPAMR"), FlowEvent::MISC_PTFX, LOC("PTFXSPAMR_H"));
			}

			this->createChild<CommandRidTools>();

			// SC DM Notifications
			{
				auto sc_dm_notifs = this->createChild<CommandList>(LOC("SCDMT"));

				sc_dm_notifs->createChild<CommandTogglePointer>(&g_hooking.scdm_scnotify, LOC("SCNOTIF"), CMDNAMES_OBF("scdmscnotify"));
				g_hooking.scdm_toasts.populateList(sc_dm_notifs, CMDNAMES_OBF("scdm"));
			}

			// CEO/MC
			{
				auto ceo = this->createChild<CommandList>(LOC("CEO"));

				{
					auto colour_slots = ceo->createChild<CommandList>(LOC("CEOCOL"), { CMDNAME("ceocolours") });

					for (int i = 0; i < 10; ++i)
					{
						colour_slots->createChild<CommandCeoColourSlot>(i);
					}
				}

				ceo->createChild<CommandInteractionMenuClick>(LOC("ORGCR_S"), CMDNAMES("ceostart", "startceo", "startorg"), GLOBAL_AM_PI_MENU_SUBMENU_CEO);
				ceo->createChild<CommandInteractionMenuClick>(LOC("ORGCR_M"), CMDNAMES("mcstart", "startmc"), GLOBAL_AM_PI_MENU_SUBMENU_MC);
				ceo->createChild<CommandCeoName>();
				ceo->createChild<CommandCeoType>();
				//ceo->createChild<CommandCeoDisableCap>();
				ceo->createChild<CommandLambdaActionScript>(LOC("CEOAPPEAL"), { CMDNAME("ceoappeal"), CMDNAME("mcappeal") }, NOLABEL, [](const Click& click)
				{
					if (click.inOnline())
					{
						ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_BANNED_SINCE).set<int>(0);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_VIPGAMEPLAYDISABLEDTIMER"), 0, true);
					}
				});
			}

			this->createChild<CommandListMental>();

			// ATM
			{
				auto atm = this->createChild<CommandList>(LOC("ATM"));

				atm->createChild<CommandWithdraw>();
				atm->createChild<CommandWalletHopper>();
			}

			// Tunables
			{
				auto tunables = this->createChild<CommandList>(LOC("TNBL"), { CMDNAME("tunables") }, LOC("TNBL_H"));

				tunables->createChild<CommandDisableInsurancePay>();
				tunables->createChild<CommandHalloween>();
				tunables->createChild<CommandPeyotes>();
			}
		}
		this->createChild<CommandLambdaAction>(LOC("QTSP"), { CMDNAME("quittosp"), CMDNAME("quittostorymode"), CMDNAME("quitostorymode"), CMDNAME("quittosingleplayer"), CMDNAME("quitosingleplayer"), CMDNAME("quitosp") }, NOLABEL, [](Click& click)
		{
			if (click.inOnline())
			{
				click.ensureYieldableScriptThread([]
				{
					TransitionHelper::doTransition(STAND_QUIT_TO_SP);
				});
			}
		});
		this->createChild<CommandLambdaAction>(LOC("QTSPF"), { CMDNAME("forcequittosp"), CMDNAME("forcequittostorymode"), CMDNAME("forcequitostorymode"), CMDNAME("forcequittosingleplayer"), CMDNAME("forcequitosingleplayer"), CMDNAME("forcequitosp") }, LOC("QTSPF_H"), [](Click& click)
		{
			click.ensureYieldableScriptThread([]
			{
				TransitionHelper::forceQuit();
				while (*pointers::CLoadingScreens_ms_Context == 0)
				{
					Script::current()->yield();
				}
				while (*pointers::CLoadingScreens_ms_Context != 0)
				{
					Script::current()->yield();
				}
				evtForceQuitting::trigger({});
			});
		});
		if (g_gui.isRootStateFull())
		{
			// Request Services
			{
				auto request_services = this->createChild<CommandList>(LOC("REQSRV"));

				request_services->createChild<CommandActionOnlineGlobal>(GLOBAL_AM_AIRSTRIKE, LOC("AIRSTRK"), CMDNAMES("airstrike", "requestairstrike"), LOC("AIRSTRK_H"));
				request_services->createChild<CommandActionOnlineGlobal>(GLOBAL_AM_AMMO_DROP, LOC("AM_AMMO_DROP"), CMDNAMES("requestammodrop"));
				request_services->createChild<CommandActionOnlineGlobal>(GLOBAL_AM_BOAT_TAXI, LOC("AM_BOAT_TAXI"), CMDNAMES("boatpickup", "boattaxi", "boataxi", "requestboatpickup"));
				request_services->createChild<CommandHeliTaxi>();
				request_services->createChild<CommandActionOnlineGlobal>(GLOBAL_AM_BACKUP_HELI, LOC("AM_BACKUP_HELI"), CMDNAMES("helibackup", "backuphelicopter", "requesthelibackup", "requestbackuphelicopter"));
				request_services->createChild<CommandRequestNanoDrone>();
			}

			this->createChild<CommandOtr>();
			this->createChild<CommandBst>();
			this->createChild<CommandBstOnce>();
			this->createChild<CommandRemoveBounty>();
			this->createChild<CommandRevealOtr>();
			this->createChild<CommandRevealInvisible>();
			this->createChild<CommandBecomeOrbitalCannon>();
			this->createChild<CommandReportStats>();
			this->createChild<CommandAimMode>();
			this->createChild<CommandAdminDlc>();
			this->createChild<CommandBlipTags>();
			this->createChild<CommandLambdaActionScript>(LOC("RSTHDSHT"), CMDNAMES("resetheadshots", "resetpedheadshots"), LOC("RSTHDSHT_H"), [](const Click&)
			{
				for (int i = 0; i != 100; ++i)
				{
					PED::UNREGISTER_PEDHEADSHOT(i);
				}
			});
		}
	}
}
