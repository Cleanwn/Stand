#include "ScriptEventTaxonomy.hpp"

#include <fmt/core.h>

#include <soup/ObfusString.hpp>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "AbstractPlayer.hpp"
#include "Auth.hpp"
#include "AptHelper.hpp"
#include "CNetGamePlayer.hpp"
#include "CNetworkSession.hpp"
#include "CommandPlayer.hpp"
#include "eMpMission.hpp"
#include "FlowEvent.hpp"
#include "FlowEventReaction.hpp"
#include "get_current_time_millis.hpp"
#include "is_session.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "script_events.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"
#include "TransitionHelper.hpp"
#include "Tunables.hpp"
#include "Util.hpp"

#define MAX_INTERIOR_ID g_tunables.getInt(TUNAHASH("MAX_INTERIOR_ID"))
#define INTERIOR_ARRAY_SIZE MAX_INTERIOR_ID + 1

namespace Stand
{
	static time_t user_became_target_rival_at = 0;
	static time_t user_terminated_at = 0;

	bool ScriptEventTaxonomy::isDirectPacket(int64_t* _args, int32_t _args_count)
	{
		return _args_count > ARGSCOUNT2(3)
			&& ARGSIDX2(0) == g_auth.direct_packet_magic
			;
	}

	EventAggregation ScriptEventTaxonomy::dissect(int64_t* _args, int32_t _args_count, std::unordered_map<int32_t, std::string>& index_names, player_t sender_id)
	{
		EventAggregation res(LOC("SE"));
		if (_args_count < 1)
		{
			res.add(FlowEvent::SE_INVALID, "S1");
		}
		else
		{
			bool ignore_wrong_metadata = true;
			switch (ARGSIDX2(0))
			{
			default:
				ignore_wrong_metadata = (sender_id >= MAX_PLAYERS);
				break;

			case -1890951223: // {0: -1890951223, 1: 15, 2: 48}
				res.addSelfIdentification(soup::ObfusString("Explosive").str());
				break;

			case 7263263744:
			case 7263263749:
			case 72632637999:
			case 74658346534:
			case 73858735564:
			case 7263263764:
			case 7263268573:
			case 8364956390:
			case 7263438154:
			case 7263437100:
			case -123536367:
			case -2289045:
			case 671111221218868:
				res.add(FlowEvent::SE_INVALID, "S2");
				break;
			}
			switch ((int32_t)ARGSIDX2(0))
			{
			case SE_NOTIFY:
				if (_args_count != ARGSCOUNT2(14))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else
				{
					if (auto cmd = AbstractPlayer(sender_id).getCommand())
					{
						if (cmd->notification_ratelimit.canRequest())
						{
							cmd->notification_ratelimit.addRequest();
						}
						else
						{
							if ((int32_t)ARGSIDX2(2) == SE_NOTIFY_COLLECHPNT)
							{
								res.add(FlowEvent::SE_CRASH, "S7");
							}
							else
							{
								res.add(FlowEvent::SE_INVALID, "S7");
							}
						}
						if ((int32_t)ARGSIDX2(2) == SE_NOTIFY_COLLECHPNT)
						{
							if (cmd->sound_se_ratelimit.canRequest())
							{
								cmd->sound_se_ratelimit.addRequest();
							}
							else
							{
								res.add(FlowEvent::SE_INVALID, "SA");
							}
						}
					}
					switch ((int32_t)ARGSIDX2(2))
					{
					case SE_NOTIFY_AMCH_POS:
						if ((uint32_t)ARGSIDX2(10) >= MAX_PLAYERS)
						{
							res.add(FlowEvent::SE_KICK, "S3");
						}
						break;

					case SE_NOTIFY_EWO:
						if ((uint32_t)ARGSIDX2(10) >= MAX_PLAYERS)
						{
							res.add(FlowEvent::SE_INVALID, "S3");
						}
						else if (!ignore_wrong_metadata
							&& (uint32_t)ARGSIDX2(10) != sender_id
							)
						{
							res.add(FlowEvent::SE_INVALID, "S4");
						}
						break;

					case SE_NOTIFY_BANK:
					case SE_NOTIFY_STOL:
					case SE_NOTIFY_REMO:
					case SE_NOTIFY_1NOWK:
					case SE_NOTIFY_INCWANTED:
					case SE_NOTIFY_ENTERED:
						res.add(FlowEvent::SE_INVALID, "S5");
						break;

					case SE_NOTIFY_SLVGE_BNNR_1:
					case SE_NOTIFY_SLVGE_BNNR_2:
						if (GtaThread::fromHash(ATSTRINGHASH("gb_salvage")) == nullptr)
						{
							res.add(FlowEvent::SE_INVALID, "S6");
						}
						break;

					case SE_NOTIFY_STOLSTOR:
						switch ((uint32_t)ARGSIDX2(3))
						{
						case 229:
						case 233:
						case 158:
						case 256:
						case 263:
							break;

						default:
							res.add(FlowEvent::SE_NOTIFY_EMPTY);
							break;
						}
						break;

					case SE_NOTIFY_EMPTY_1:
					case SE_NOTIFY_EMPTY_2:
					case SE_NOTIFY_EMPTY_3:
					case SE_NOTIFY_EMPTY_4:
					case SE_NOTIFY_EMPTY_5:
						res.add(FlowEvent::SE_NOTIFY_EMPTY);
						break;
					}
				}
				break;

			case SE_POINT_RACE_INV:
			case SE_DEATHMATCH_INV:
			case SE_GANG_INV:
				if (auto cmd = AbstractPlayer(sender_id).getCommand())
				{
					if (cmd->sound_se_ratelimit.canRequest())
					{
						cmd->sound_se_ratelimit.addRequest();
					}
					else
					{
						res.add(FlowEvent::SE_INVALID, "SA");
					}
				}
				break;

			case SE_INT_INVITE:
				if (_args_count != ARGSCOUNT2(5))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(2) && uint32_t(ARGSIDX2(2)) >= INTERIOR_ARRAY_SIZE + 1) // global size + 1 because game weirdly does handle the case of index being global size
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				[[fallthrough]];
			case SE_OFFICE_INVITE:
			case SE_FMC_INVITE:
				if (auto cmd = AbstractPlayer(sender_id).getCommand())
				{
					if (cmd->int_invite_ratelimit.canRequest())
					{
						cmd->int_invite_ratelimit.addRequest();
					}
					else
					{
						res.add(FlowEvent::SE_INVALID, "S8");
					}
				}
				break;

			case SE_CHAR_INVITE:
				if (sender_id != g_player)
				{
					res.add(FlowEvent::SE_INVALID, "S9");
				}
				break;

			case SE_ZERO_KICK_1:
				if (_args_count != ARGSCOUNT2(14) || (int32_t)ARGSIDX2(2) != 11)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				break;

			case SE_ZERO_KICK_2:
			case SE_ZERO_KICK_3:
			case SE_ZERO_KICK_4:
				if (_args_count != ARGSCOUNT2(53) || (int32_t)ARGSIDX2(2) != 50)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				break;

			case SE_ZERO_KICK_5:
				if (_args_count != ARGSCOUNT2(27) || (int32_t)ARGSIDX2(2) != 5 || (int32_t)ARGSIDX2(15) != 5 || (int32_t)ARGSIDX2(21) != 5)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				break;

			case SE_ZERO_KICK_6: // also seems to cause freemode death with very big params
				if (_args_count != ARGSCOUNT2(40) || (int32_t)ARGSIDX2(2) != 5 || (int32_t)ARGSIDX2(2 + 16) != 5 || (int32_t)ARGSIDX2(2 + 22) != 5 || (int32_t)ARGSIDX2(2 + 28) != 5)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				break;

			case SE_ZERO_KICK_7:
				if (_args_count != ARGSCOUNT2(31) || (int32_t)ARGSIDX2(2) < 7)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				break;

			case SE_ZERO_KICK_8:
				if (_args_count != ARGSCOUNT2(7) || (int32_t)ARGSIDX2(2) != 4)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				break;

			case SE_ZERO_KICK_9:
				if (_args_count != ARGSCOUNT2(12) || (int32_t)ARGSIDX2(2) != 4 || (int32_t)ARGSIDX2(7) != 4)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				break;

			case SE_ZERO_KICK_10:
				if (_args_count != ARGSCOUNT2(10) || (int32_t)ARGSIDX2(3) != 6)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				break;

			case SE_ZERO_KICK_11:
				// This may be triggered indirectly via SE_FM_MISSION if we're the SH, so only marking it as "unusual" instead of "kick"
				if (_args_count != ARGSCOUNT2(7)
					|| !isValidFreemodeMissionId((uint32_t)ARGSIDX2(2))
					)
				{
					res.add(FlowEvent::SE_UNUSUAL, "S0");
				}
				break;

			case SE_UNUSED_TELEPORT:
				/*if (_args_count != ARGSCOUNT2(10))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}*/
				if (_args_count <= ARGSCOUNT2(3)
					|| (uint32_t)ARGSIDX2(2) >= 123
					|| (uint32_t)ARGSIDX2(3) != 4
					)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				else
				{
					res.add(FlowEvent::SE_INVALID, "S5");
				}
				break;

			case SE_BOUNDS_KICK_10:
			case SE_BOUNDS_KICK_25:
				if (_args_count != ARGSCOUNT2(4))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(2) && uint32_t(ARGSIDX2(2)) >= 20)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_6:
			case SE_BOUNDS_KICK_8:
				if (_args_count != ARGSCOUNT2(4))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(3) && uint32_t(ARGSIDX2(3)) >= 26)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_23:
				if (_args_count != ARGSCOUNT2(6))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(3) && uint32_t(ARGSIDX2(3)) >= 79)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_12:
				if (_args_count != ARGSCOUNT2(4))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(3) && uint32_t(ARGSIDX2(3)) >= 124)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_15:
				if (_args_count != ARGSCOUNT2(7))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(5) && uint32_t(ARGSIDX2(5)) >= 32)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_17:
				if (_args_count != ARGSCOUNT2(17))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(5) && uint32_t(ARGSIDX2(5)) >= 225)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_18:
				if (_args_count != ARGSCOUNT2(17))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(4) && uint32_t(ARGSIDX2(4)) >= 225)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_19:
				if (_args_count != ARGSCOUNT2(3))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(2) && uint32_t(ARGSIDX2(2)) >= 32)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_28:
				if (_args_count != ARGSCOUNT2(3))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (!g_player.isInAnOrg())
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_20:
				if (_args_count != ARGSCOUNT2(20))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(2) && uint32_t(ARGSIDX2(2)) >= 7)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				if (_args_count > ARGSCOUNT2(3) && uint32_t(ARGSIDX2(3)) >= 4)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_27:
				if (_args_count != ARGSCOUNT2(5))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(2) && uint32_t(ARGSIDX2(2)) > 6) // The actual limit should be >= 6, but apparently in races the host uses 6 here
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_24:
				if (_args_count != ARGSCOUNT2(4))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(3) && uint32_t(ARGSIDX2(3)) >= 32)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

			case SE_BOUNDS_KICK_26:
				if (_args_count != ARGSCOUNT2(9))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(7) && (uint32_t)ARGSIDX2(7) >= MAX_PLAYERS)
				{
					res.add(FlowEvent::SE_KICK, "S3");
				}
				break;

				// unused SEs
			case SE_STAT_READ_INT:
			case SE_STAT_READ_FLOAT:
			case SE_STAT_READ_BOOL:
				res.add(FlowEvent::SE_INVALID, "S4");
				break;
			case SE_UNK_KICK_3:
				res.add(FlowEvent::SE_KICK, "S4");
				break;

			case SE_SEND_TO_CLUBHOUSE:
				if (_args_count <= ARGSCOUNT2(3) || (uint32_t)ARGSIDX2(3) != 32)
				{
					res.add(FlowEvent::SE_CRASH, "S0");
				}
				else if (_args_count != ARGSCOUNT2(37))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else if ((int32_t)ARGSIDX2(2) <= 0) // property id
				{
					res.add(FlowEvent::SE_INFLOAD);
				}
				else
				{
					bool affects_me = false;
					Hash my_player_hash = NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(g_player);
					for (uint32_t i = 0; i != (uint32_t)ARGSIDX2(3); ++i)
					{
						if (my_player_hash == (Hash)ARGSIDX2(3 + 1 + i))
						{
							affects_me = true;
						}
					}
					if (affects_me)
					{
						if (sender_id >= MAX_PLAYERS
							|| (g_player.getBoss() != sender_id
								&& g_player != sender_id
								)
							)
						{
							res.add(FlowEvent::SE_INTERIORWARP_NCEO);
						}
						else
						{
							res.add(FlowEvent::SE_INTERIORWARP);
						}
					}
				}
				break;

			case SE_SH_SPAWN_VEH: // can't be triggered on yourself
				if (_args_count != ARGSCOUNT2(8)
					|| isnan(*reinterpret_cast<float*>(&ARGSIDX2(2)))
					|| isnan(*reinterpret_cast<float*>(&ARGSIDX2(3)))
					|| isnan(*reinterpret_cast<float*>(&ARGSIDX2(4)))
					)
				{
					res.add(FlowEvent::SE_CRASH, "S2");
				}
				break;

			case SE_UNK_KICK_2:
				if (_args_count != ARGSCOUNT2(40) || (uint32_t)ARGSIDX2(2 + 3) != 6)
				{
					res.add(FlowEvent::SE_KICK, "S2");
				}
				break;

			case SE_NETWORK_BAIL:
				res.add(FlowEvent::SE_KICK, "S1");
				break;

			case SE_UNK_KICK_1:
				res.add(FlowEvent::SE_KICK, "S2");
				break;

			case SE_PV_KICK:
			case SE_PV_DESTROY:
				res.add(FlowEvent::SE_PVKICK);
				break;

			case SE_PV_CLEANUP:
				if (_args_count != ARGSCOUNT2(5))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (sender_id < MAX_PLAYERS
					&& sender_id != g_player
					)
				{
					// Note: The script "network id" is not the same as netObject id!
					auto our_pv_net_id = ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(g_player, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_netID_PV).get<int>();
					auto their_pv_net_id = ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(sender_id, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_netID_PV).get<int>();
					if (our_pv_net_id != NETWORK_INVALID_OBJECT_ID
						&& their_pv_net_id == our_pv_net_id
						)
					{
						res.add(FlowEvent::SE_PVKICK);
					}
				}
				break;

			case SE_PV_WARP:
				if (g_player_veh.isValid())
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				break;

			case SE_SEND_TO_JOB_2:
				if (_args_count != ARGSCOUNT2(24))
				{
					index_names.emplace(0, LANG_GET("SND2JB"));
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count <= ARGSCOUNT2(2) || (uint32_t)ARGSIDX2(2) != 21)
				{
					index_names.emplace(0, LANG_GET("SND2JB"));
					res.add(FlowEvent::SE_KICK, "S0");
				}
				[[fallthrough]];
			case SE_SEND_TO_JOB:
				res.add(FlowEvent::SE_SENDTOJOB);
				break;

			case SE_SEND_TO_JOB_3: // Session scripts for poor people: https://github.com/YimMenu/YimMenu/blob/master/src/backend/commands/player/toxic/start_script.cpp
				if (is_session_freeroam())
				{
					if ((int32_t)ARGSIDX2(2) == fm_hideout_controler && sender_id == AbstractPlayer::script_host)
					{
						// Script host may instruct us to start fm_hideout_controler for gang attack, this is fine.
					}
					else
					{
						res.add(FlowEvent::SE_SENDTOJOB);
					}
				}
				break;

			case SE_CEO_KICK:
				if (g_player.getBoss() != sender_id)
				{
					res.add(FlowEvent::SE_CEOKICK);
				}
				break;

			case SE_MARK_AS_BEAST:
				if (_args_count != ARGSCOUNT2(3))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else if (AbstractPlayer((player_t)ARGSIDX2(2)).isValid())
				{
					if (auto t = GtaThread::fromHash(ATSTRINGHASH("am_hunt_the_beast"));
						!t
						|| t->getHost() != sender_id
						|| *ScriptLocal(t, LOCAL_HTB_BEAST).as<player_t*>() != (player_t)ARGSIDX2(2)
						)
					{
						if (g_player.getBoss() == (player_t)ARGSIDX2(2))
						{
							res.add(FlowEvent::SE_CEOKICK);
						}
						/*else
						{
							res.add(FlowEvent::SE_INVALID, "S4");
						}*/
					}
				}
				break;

			case SE_ENTER_WAREHOUSE:
				if (_args_count != ARGSCOUNT2(5))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else if ((int)ARGSIDX2(2) == 0
					&& (int)ARGSIDX2(3) == 1
					&& (int)ARGSIDX2(4) == 0
					)
				{
					res.add(FlowEvent::SE_FREEZE, "S0");
				}
				else if (sender_id >= MAX_PLAYERS
					|| (g_player.getBoss() != sender_id
						&& g_player != sender_id
						)
					|| g_player.isInMc()
					)
				{
					res.add(FlowEvent::SE_INTERIORWARP_NCEO);
				}
				else
				{
					res.add(FlowEvent::SE_INTERIORWARP);
				}
				break;

				// R* checks for CEO/MC membership + distance, but these events should only be sent by our boss.
			case SE_CASINO_HEI_CUTSCENE:
			case SE_CAYO_WARP:
				if (sender_id >= MAX_PLAYERS
					|| (g_player.getBoss() != sender_id
						&& g_player != sender_id
						)
					)
				{
					res.add(FlowEvent::SE_INTERIORWARP_NCEO);
				}
				else
				{
					res.add(FlowEvent::SE_INTERIORWARP);
				}
				break;

			case SE_GIVE_COLLECTIBLE:
				if (_args_count != ARGSCOUNT2(7))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else if (ARGSIDX2(2) == 4 && ARGSIDX2(3) >= 25)
				{
					res.add(FlowEvent::SE_KICK, "S0");
				}
				else if (sender_id >= MAX_PLAYERS
					|| (g_player.getBoss() != sender_id
						&& g_player != sender_id
						)
					)
				{
					res.add(FlowEvent::SE_GIVECOLLE_NCEO);
				}
				else
				{
					res.add(FlowEvent::SE_GIVECOLLE);
				}
				break;

			case SE_BOUNTY:
				res.setName(LOC("BNTY"));
				index_names.emplace(ARGSCOUNT2(2), "Target");
				index_names.emplace(ARGSCOUNT2(4), "Value");
				index_names.emplace(ARGSCOUNT2(6), "From NPC");
				if (const auto sender = AbstractPlayer(sender_id); sender.isValid())
				{
					if (_args_count != ARGSCOUNT2(22))
					{
						res.add(FlowEvent::SE_INVALID, "S3");
					}
					else
					{
						const auto target = AbstractPlayer((uint32_t)ARGSIDX2(2));

						// Bounty Reactions
						if (target == g_player)
						{
							auto reactions = sender.getReactions(FlowEvent::MISC_BOUNTY);
							const auto toast_flags = flow_event_reactions_to_toast_flags(reactions);
							Util::toast(LANG_FMT("BOUNTYR_P", sender.getName(), (int)ARGSIDX2(4)), toast_flags);
							sender.applyReactions(reactions);
						}

						if (sender != target)
						{
							// Bounty Spam
							SOUP_IF_LIKELY(auto cmd = sender.getCommand())
							{
								if (cmd->bounty_ratelimit.canRequest())
								{
									cmd->bounty_ratelimit.addRequest();
								}
								else
								{
									sender.triggerDetection(FlowEvent::MOD_BOUNTYSPAM);
								}
							}

							// Modded Bounty Amount
							const auto value = (int)ARGSIDX2(4);
							const auto lester_cut = ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LESTER_BOUNTY_CUT).get<int>();
							switch (value + lester_cut)
							{
							case 2000:
							case 4000:
							case 6000:
							case 8000:
							case 10000:
								break;

							default:
								sender.triggerDetection(FlowEvent::MOD_SUSSYBOUNTY, fmt::format("${}", value));
							}
						}
					}
				}
				break;

			case SE_CLEAR_WANTED:
				res.setName(LOC("PTX_BAIL"));
				break;

			case SE_START_TYPING:
				res.setName(LOC("PTX_TYPNGSTRT"));
				if (const auto sender = AbstractPlayer(sender_id); sender.isValid())
				{
					SOUP_IF_LIKELY(auto cmd = sender.getCommand())
					{
						cmd->last_typing_at = get_current_time_millis();
					}
				}
				break;

			case SE_STOP_TYPING:
				res.setName(LOC("PTX_TYPNGSTOP"));
				if (const auto sender = AbstractPlayer(sender_id); sender.isValid())
				{
					SOUP_IF_LIKELY(auto cmd = sender.getCommand())
					{
						cmd->has_silentchat_chance = false;
					}
				}
				break;

			case SE_APARTMENT_INVITE:
				index_names.emplace(ARGSCOUNT2(2), "Owner");
				index_names.emplace(ARGSCOUNT2(5), "Property");
				if (_args_count > ARGSCOUNT2(2) && (uint32_t)ARGSIDX2(2) >= MAX_PLAYERS)
				{
					index_names.emplace(0, LANG_GET("PTX_APT"));
					res.add(FlowEvent::SE_KICK, "S5");
				}
				else if (_args_count > ARGSCOUNT3(4) && (int)ARGSIDX3(4) != -1)
				{
					// Infinite Loading Screen: {0: -1321657966, 1: 0, 2: 2, 3 (Owner): 0, 4: 1, 5: -1, 6 (Property): 1, 7: -1, 8: 0, 9: 0, 10: 0}
					// Legit Invite: {0: 2575658719634 (-1321657966), 1: 2572685410326 (22), 2: 33554432, 3 (Owner): 22, 4: -1, 5: -1, 6 (Property): 88, 7: 0, 8: 0, 9: 0, 10: 0}
					index_names.emplace(0, LANG_GET("PTX_APT"));
					res.add(FlowEvent::SE_INFLOAD);
				}
				else if (_args_count > ARGSCOUNT2(5) &&
					(
						(uint32_t)ARGSIDX2(5) == 0
						|| (uint32_t)ARGSIDX2(5) > AptHelper::max_property_id
						)
					)
				{
					index_names.emplace(0, LANG_GET("PTX_APT"));
					if (AbstractPlayer((uint32_t)ARGSIDX2(2)).exists())
					{
						res.add(FlowEvent::SE_INFLOAD);
					}
					else
					{
						res.add(FlowEvent::SE_DISABLEDRIVINGVEHICLES);
					}
				}
				else if (_args_count != ARGSCOUNT2(10))
				{
					index_names.emplace(0, LANG_GET("PTX_APT"));
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else
				{
					res.add(FlowEvent::SE_APTINV);
				}
				break;

			case SE_WARP_INTERIOR:
			{
				bool valid = true;
				if (_args_count != ARGSCOUNT2(15)
					|| (int32_t)ARGSIDX2(5) != 1
					)
				{
					res.add(FlowEvent::SE_INVALID, "S3");
					valid = false;
				}
				if (_args_count > ARGSCOUNT2(2) && (uint32_t)ARGSIDX2(2) > MAX_INTERIOR_ID)
				{
					res.add(FlowEvent::SE_KICK, "S3");
					valid = false;
				}
				if (valid
					&& (uint32_t)ARGSIDX2(4) == 0
					)
				{
					if (sender_id >= MAX_PLAYERS
						|| (g_player.getBoss() != sender_id
							&& g_player != sender_id
							)
						)
					{
						res.add(FlowEvent::SE_INTERIORWARP_NCEO);
					}
					else
					{
						res.add(FlowEvent::SE_INTERIORWARP);
					}
				}
			}
			break;

			case SE_CAYO_PERICO_INVITE:
				/*if (args_count <= 3 || (uint32_t)ARGSIDX2(3) == 0)
				{
					res.add(FlowEvent::SE_CAYO_FORCE);
					res.setName(LOC("PLYVACATE"));
				}
				else*/
			{
				res.add(FlowEvent::SE_CAYO_INV);
			}
			break;

			case SE_OTR:
				res.setName(LOC("OTR"));
				break;

			case SE_GAME:
				if (_args_count != ARGSCOUNT2(4))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				if (_args_count > ARGSCOUNT2(2))
				{
					if ((uint32_t)ARGSIDX2(2) == SE_GAME_FORCECAMFRONT)
					{
						if (is_session_freeroam())
						{
							res.add(FlowEvent::SE_FORCECAMFRONT);
						}
					}
					else if ((uint32_t)ARGSIDX2(2) == SE_GAME_HTBIDLEHELP)
					{
						// Can be abused for sound spam. Caught by PCM patch.
						if (auto cmd = AbstractPlayer(sender_id).getCommand())
						{
							if (GET_MILLIS_SINCE(cmd->last_htbidlehelp) < 3'000)
							{
								res.add(FlowEvent::SE_CRASH, "S4");
							}
							cmd->last_htbidlehelp = get_current_time_millis();
						}
					}
				}
				break;

			case SE_UNUSED_LABEL_SMS:
			case SE_UNUSED_EMPTY_NOTIFY: // can also be used to destroy personal vehicle
				res.add(FlowEvent::SE_INVALID, "S5");
				break;

			case SE_FM_MISSION:
				if (_args_count > ARGSCOUNT2(2))
				{
					if (isValidFreemodeMissionId((uint32_t)ARGSIDX2(2)))
					{
						if (sender_id >= MAX_PLAYERS
							|| (g_player.getBoss() != sender_id
								&& g_player != sender_id
								)
							)
						{
							res.add(FlowEvent::SE_STARTFMMISSION_NCEO);
						}
						else
						{
							res.add(FlowEvent::SE_STARTFMMISSION);
						}
					}
					else
					{
						res.add(FlowEvent::SE_KICK, "S5");
					}
				}
				if (_args_count != ARGSCOUNT2(4))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				break;

			case SE_JOINED_JOB_NOTIFY:
				SOUP_IF_UNLIKELY(!g_player_veh.isValid()
					|| g_player_veh != AbstractPlayer(sender_id).getVehicle()
				)
				{
					res.add(FlowEvent::SE_INVALID, "S5");
				}
				break;

			case SE_UNK_APT_BLACKSCREEN:
				if (sender_id != g_player) // https://github.com/calamity-inc/Stand-Feedback/issues/241
				{
					SOUP_IF_UNLIKELY(_args_count != ARGSCOUNT2(5)
						|| ARGSIDX2(1) != ARGSIDX2(3)
						|| ARGSIDX2(2) == g_player
						|| ((ARGSIDX2(4) >> g_player) & 1) == 0
					)
					{
						res.add(FlowEvent::SE_INFLOAD);
					}
				}
				break;

			case SE_KICK_FROM_INTERIOR:
				SOUP_IF_UNLIKELY(_args_count != ARGSCOUNT2(8))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				res.add(FlowEvent::SE_INTERIORKICK);
				break;

			case SE_TRIGGER_DEFEND:
				SOUP_IF_UNLIKELY(_args_count != ARGSCOUNT2(3))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				SOUP_IF_UNLIKELY(sender_id != AbstractPlayer::script_host)
				{
					res.add(FlowEvent::SE_INVALID, "S4");
				}
				res.add(FlowEvent::SE_TRIGGERDEFEND);
				break;

			case SE_BOUNTY_OVER:
				// Sent by bounty-haver to inform that they no longer have a bounty.
				// When SH receives this event, they remove the bounty from the current bounties array, which can cause us to think the sender no longer has a bounty when receiving this event when they're the SH.
				// The game does delay giving the actual bounty and gets the value from GlobalplayerBD_FM_3's iBountyValue then.
				SOUP_IF_UNLIKELY(_args_count != ARGSCOUNT2(22))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else
				{
					//if (((uint32_t)ARGSIDX2(3) & (1 << 0)) // IS_BIT_SET(event.iResultBS, BOUNTY_RESULT_SUCCESS)
					//	&& (player_t)ARGSIDX2(2) == g_player // We are to get the reward?
					//	)

					SOUP_IF_LIKELY(auto cmd = AbstractPlayer(sender_id).getCommand())
					{
						if (GET_MILLIS_SINCE(cmd->last_had_bounty_at) > 10000
							|| cmd->last_bounty_value != (int)ARGSIDX2(4)
							)
						{
							if (TransitionHelper::getTimeSinceTransitionEnded() > 3000)
							{
								res.add(FlowEvent::SE_INVALID, "S4");
							}
						}
					}
				}
				break;

			case SE_START_TARGET_RIVAL:
				SOUP_IF_UNLIKELY(_args_count != ARGSCOUNT2(4))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else
				{
					if ((player_t)ARGSIDX2(2) == g_player)
					{
						user_became_target_rival_at = get_current_time_millis();
					}
				}
				break;

			case SE_CEO_CLEAR:
				SOUP_IF_UNLIKELY(_args_count != ARGSCOUNT2(4))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else
				{
					if ((BOOL)ARGSIDX2(2) == TRUE // DoTicker
						&& (int)ARGSIDX2(3) == 6 // eGBENDBEINGGOONREASON_TERMINATED
						)
					{
						user_terminated_at = get_current_time_millis();
					}
				}
				break;

			case SE_SET_TERM_TARGET:
				SOUP_IF_UNLIKELY(_args_count != ARGSCOUNT2(3))
				{
					res.add(FlowEvent::SE_INVALID, "S3");
				}
				else
				{
					// TRUE would block our passive mode
					if ((BOOL)ARGSIDX2(2))
					{
						if (GET_MILLIS_SINCE(user_became_target_rival_at) > 30'000
							&& GET_MILLIS_SINCE(user_terminated_at) > 30'000
							)
						{
							res.add(FlowEvent::SE_INVALID, "S4");
						}
					}
					else
					{
#ifdef STAND_DEBUG
						if (GET_MILLIS_SINCE(user_became_target_rival_at) > 303'000) // 5m3s
						{
							res.add(FlowEvent::SE_INVALID, "S5");
						}
#endif
					}
				}
				break;
			}
			if (!ignore_wrong_metadata)
			{
				if ((player_t)ARGSIDX2(1) != sender_id)
				{
					switch ((int)ARGSIDX2(0))
					{
					case SE_NO_SENDER_1:
					case SE_NO_SENDER_2:
					case SE_NO_SENDER_3:
					case SE_NO_SENDER_4:
					case SE_NO_SENDER_5:
					case SE_NO_SENDER_6:
					case SE_NO_SENDER_7:
						break;

					default:
						res.types.emplace_back(FlowEvent::SE_INVALID);
						if (res.name == LOC("SE"))
						{
							res.setName(FlowEvent::SE_INVALID, "S0");
						}
					}
				}

				if (sender_id != g_player)
				{
					// We just received this event, so we should be in the receiver bitflags.
					if (((ARGSIDX3(2) >> g_player) & 1) == 0)
					{
						res.types.emplace_back(FlowEvent::SE_INVALID);
						if (res.name == LOC("SE"))
						{
							res.setName(FlowEvent::SE_INVALID, "SB");
						}
					}
				}
			}
		}
		return res;
	}

	bool ScriptEventTaxonomy::isValidFreemodeMissionId(uint32_t id)
	{
		switch (id)
		{
		case 142:
		case 148:
		case 151:
		case 152:
		case 153:
		case 154:
		case 155:
		case 157:
		case 158:
		case 159:
		case 160:
		case 162:
		case 163:
		case 164:
		case 166:
		case 167:
		case 168:
		case 169:
		case 170:
		case 171:
		case 172:
		case 173:
		case 178:
		case 179:
		case 180:
		case 182:
		case 183:
		case 185:
		case 186:
		case 188:
		case 189:
		case 190:
		case 191:
		case 192:
		case 193:
		case 194:
		case 195:
		case 197:
		case 198:
		case 199:
		case 200:
		case 201:
		case 205:
		case 207:
		case 208:
		case 209:
		case 210:
		case 214:
		case 215:
		case 216:
		case 217:
		case 218:
		case 219:
		case 220:
		case 221:
		case 225:
		case 226:
		case 227:
		case 229:
		case 230:
		case 233:
		case 237:
		case 238:
		case 239:
		case 240:
		case 241:
		case 242:
		case 243:
		case 244:
		case 248:
		case 249:
		case 250:
		case 256:
		case 258:
		case 262:
		case 263:
		case 264:
		case 267:
		case 271:
		case 276:
		case 277:
		case 291:
		case 292:
		case 293:
		case 294:
		case 295:
		case 296:
		case 297:
		case 298:
		case 299:
		case 300:
		case 301:
		case 304:
		case 305:
		case 306:
		case 307:
		case 308:
		case 309:
		case 312:
		case 313:
		case 315:
		case 316:
		case 317:
		case 318:
		case 322:
		case 323:
		case 324:
		case 325:
		case 326:
		case 327:
		case 328:
		case 329:
		case 330:
		case 331:
		case 332:
		case 333:
		case 334:
		case 335:
		case 336:
		case 337:
		case 338:
		case 339:
		case 340:
			return true;
		}
		// 1.68-3095, freemode, func_8858
		return false;
	}
}
