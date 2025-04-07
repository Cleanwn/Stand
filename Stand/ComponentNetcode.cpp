#include "ComponentNetcode.hpp"

#include "AbstractEntity.hpp"
#include "AddressGamers.hpp"
#include "atArray.hpp"
#include "BattlEyeServer.hpp"
#include "be_delegate.hpp"
#include "CNetGamePlayer.hpp"
#include "CNetworkSession.hpp"
#include "CommandBlockBlockJoin.hpp"
#include "CommandPlayer.hpp"
#include "datBitBuffer.hpp"
#include "eEntityType.hpp"
#include "EventLabel.hpp"
#include "Exceptional.hpp"
#include "FlowEvent.hpp"
#include "FlowEventReaction.hpp"
#include "fmt_arg.hpp"
#include "gta_entity.hpp"
#include "gta_net_object_mgr.hpp"
#include "gta_ped.hpp"
#include "Hooking.hpp"
#include "ice_messages.hpp"
#include "IceQueen.hpp"
#include "is_rid.hpp"
#include "is_session.hpp"
#include "LeaveReasons.hpp"
#include "netComplaintMsg.hpp"
#include "netConnection.hpp"
#include "netEndpoint.hpp"
#include "netEvent.hpp"
#include "netMessageId.hpp"
#include "netTimeSync.hpp"
#include "NetworkSessionMessages.hpp"
#include "pointers.hpp"
#include "RageConnector.hpp"
#include "script_handler.hpp"
#include "script_id.hpp"
#include "script_thread.hpp"
#include "snet_messages.hpp"
#include "Tunables.hpp"
#include "Util.hpp"

#ifdef STAND_DEBUG
#include <fmt/core.h>
#include "FileLogger.hpp"
#endif

#include "ComponentImpl.hpp"

namespace Stand
{
	static DetourHook rage_netConnectionManager_Channel_DispatchEvent_hook{ "Q0" };

	//static bool being_host_kicked = false;

	static void onCriticalResponseReceived(std::vector<SessionPlayer>& pending)
	{
		if (packet_src.isValid())
		{
			const auto rid = packet_src.getRockstarId();
			for (auto i = pending.begin(); i != pending.end(); ++i)
			{
				if (i->rid == rid)
				{
					pending.erase(i);
					break;
				}
			}
		}
	}

	[[nodiscard]] static bool processKick(const AddressGamers::Peer* peer, const rage::netSocketAddress& sender_addr, const Codename& extra)
	{
		auto label = Label::combineWithBrackets(LOC("PTX_KICK"), LIT(extra.toString()));
		if (packet_src.isValid())
		{
			auto reactions = packet_src.getReactions(FlowEvent::SE_KICK);
			packet_src.applyReactionsIn(std::move(label), { FlowEvent::SE_KICK }, reactions);
			if (!(reactions & REACTION_BLOCK))
			{
				return false;
			}
		}
		else
		{
			Util::toast(LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", label.getLocalisedUtf8()), FMT_ARG("player", AddressGamers::getNameForUser(peer, sender_addr.ip))));
		}
		return true;
	}

	[[nodiscard]] static bool processRateLimitedPacket(const AddressGamers::Peer* peer, const rage::netSocketAddress& sender_addr, PacketRateLimit& data, const Codename& codename)
	{
		if (data.ratelimit.canRequest())
		{
			data.ratelimit.addRequest();
			data.did_reaction = false;
		}
		else
		{
			// If this is spammed a thousand times, we don't wanna react a thousand times as well, especially with reactions like "Crash."
			SOUP_IF_UNLIKELY (!data.did_reaction)
			{
				data.did_reaction = true;
				data.should_block = processKick(peer, sender_addr, codename);
			}
			if (data.should_block)
			{
				return true;
			}
		}
		return false;
	}

	void __fastcall rage_netConnectionManager_Channel_DispatchEvent(rage::netConnectionManager::Channel* channel, rage::netConnectionManager* con_mgr, rage::netEvent* event)
	{
		__try
		{
			THREAD_NAME("Net Recv?");

			const auto eventId = event->GetId();
			if (eventId == rage::NET_EVENT_FRAME_RECEIVED)
			{
				auto packet = reinterpret_cast<rage::netConnection::InFrame*>(event);

				rage::netSocketAddress sender_addr{};
				if (packet->endpoint_id != -1)
				{
					if (auto endpoint = con_mgr->getEndpointById(packet->endpoint_id))
					{
						sender_addr = endpoint->getPublicAddress();
						packet_src = AbstractPlayer::fromEndpointId(packet->endpoint_id);
					}
					else if (packet->sender.sock_addr.ip != -1)
					{
						sender_addr = packet->sender.sock_addr;
						packet_src = AbstractPlayer::fromEndpointAddress(sender_addr);
					}
					else
					{
						// In case of NET_ICE_SESSION_ANSWER, we often have an endpoint id, but it does not resolve to an endpoint.
						packet_src = AbstractPlayer::invalid();
					}
				}
				else if (packet->sender.sock_addr.ip != -1)
				{
					sender_addr = packet->sender.sock_addr;
					packet_src = AbstractPlayer::fromEndpointAddress(sender_addr);
				}
				else
				{
					// I was not able to observe any cases where con_id was not -1 here.
					packet_src = AbstractPlayer::invalid();
				}

				rage::datBitBuffer buf{};
				buf.SetReadOnlyBytes(packet->data, packet->size);
				const auto netMsgId = rage::netMessage::readHeader(buf);
#ifdef STAND_DEBUG
				std::string what = "Neither a message nor a response";
				const char* transactor_format = nullptr;
#endif
				if (netMsgId == -1)
				{
					buf.seekStart();
					uint32_t sig = buf.readU32(20);
					bool is_transactor_packet = false;
					if (sig == 0x8924F) // transactor response
					{
						is_transactor_packet = true;
#ifdef STAND_DEBUG
						transactor_format = "{} (in response to txid {})";
#endif
					}
					else if (sig == 0x8C253) // transactor request
					{
						is_transactor_packet = true;
#ifdef STAND_DEBUG
						transactor_format = "{} (as request with txid {})";
#endif
					}
					if (is_transactor_packet)
					{
						uint32_t txid = buf.readU32();
						buf.SetCursorPos(7 * 8); // R* doesn't do this in a single go, so they end up aligning to the next byte (7) after the transaction header.
						auto innerNetMsgId = rage::netMessage::readHeader(buf);
#ifdef STAND_DEBUG
						what = fmt::format(fmt::runtime(transactor_format), rage::netMessage_getName(innerNetMsgId), txid);
#endif
						CONST_ASSIGN(netMsgId, innerNetMsgId);
					}
				}
#ifdef STAND_DEBUG
				else
				{
					what = rage::netMessage_getName(netMsgId);
				}
				if (g_comp_netcode.log_packet_receive)
				{
					std::string src_name{};
					if (packet_src.isValid())
					{
						src_name = packet_src.getName();
						src_name.append(" (");
						src_name.append(sender_addr.ip.toString<std::string>());
						src_name.push_back(')');
					}
					else if (sender_addr.ip != -1)
					{
						src_name = AddressGamers::getNameForUser(sender_addr.ip);
					}
					else
					{
						src_name = "**Invalid**";
					}
					std::string transport_info = fmt::format("on channel {}", packet->channel_id);
					if (packet->con_id == -1)
					{
						transport_info.insert(0, "out-of-band ");
					}
					g_logger.log(fmt::format("{} from {} {}", what, std::move(src_name), transport_info));
				}
				/*if ((uint32_t)netMsgId == -1 && being_host_kicked)
				{
					being_host_kicked = false;
					return;
				}*/
#endif
				// Note: This may be sent as part of the connection close event, so we can't always intercept it using this method
				if (netMsgId == SN_MSG_JOIN_RESPONSE)
				{
					auto sessid = buf.readU64();
					auto slottype = buf.readU32(2);
					auto response_code = buf.readU32(3);
					auto sizeOfGamerData = buf.readU32(10);
					auto sizeOfResponseData = buf.readU32(10);

					if (sizeOfGamerData > SNET_MAX_SIZEOF_GAMER_DATA
						|| sizeOfResponseData > SNET_MAX_SIZEOF_JOIN_RESPONSE_DATA
						)
					{
						packet_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, Codename("Q3").toString());
						return;
					}

					//Util::toast(fmt::format("Received snMsgJoinResponse for session {:X}: code = {}, slot type = {}, g.d. size = {}, r.d. size = {}", sessid, response_code, slottype, sizeOfGamerData, sizeOfResponseData), TOAST_ALL);

					/*rage::datBitBuffer bb{};
					bb.SetReadWriteBytes(packet->data, packet->size);

					rage::netMessage::WriteHeader(SN_MSG_JOIN_RESPONSE, bb);
					bb.writeU64(sessid);
					bb.writeI32(slottype, 2);
					bb.writeI32(6, 3);*/
				}
				else if (netMsgId == PACKED_RELIABLES_MSG)
				{
					const auto bitflags = buf.readU32(4);
					if (bitflags & 1) // creates?
					{
						buf.SkipBits(32); // skip timestamp
						if (buf.readU32(5))
						{
							auto msg_len = buf.readU32(13);
							SOUP_IF_UNLIKELY (msg_len > 912 * 8)
							{
								packet_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, Codename("Q2").toString());
								return;
							}
							buf.SkipBits(msg_len);
						}
					}
					if (bitflags & 2) // create acks?
					{
						if (buf.readU32(5))
						{
							auto msg_len = buf.readU32(13);
							SOUP_IF_UNLIKELY (msg_len > 912 * 8)
							{
								packet_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, Codename("Q2").toString());
								return;
							}
							buf.SkipBits(msg_len);
						}
					}
					if (bitflags & 4) // removes?
					{
						if (buf.readU32(5))
						{
							auto msg_len = buf.readU32(13);
							SOUP_IF_UNLIKELY (msg_len > 912 * 8)
							{
								packet_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, Codename("Q2").toString());
								return;
							}

							char data[912];
							buf.ReadBits(data, msg_len);

							rage::datBitBuffer pack_bb;
							pack_bb.SetReadOnlyBytes(data, msg_len);

							for (; msg_len >= 18; msg_len -= 18)
							{
								auto object_id = pack_bb.readU32(13);
								auto ownership_token = pack_bb.readU32(5);
								if (g_player_ped.isNetObjId(object_id))
								{
									auto reactions = packet_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, Codename("Q1").toString());
									if (reactions & REACTION_BLOCK)
									{
										return;
									}
								}
								else if (g_player_veh.isNetObjId(object_id))
								{
									if (g_player_veh.isOwnerOfVehicleAndDriver())
									{
										auto reactions = packet_src.getAndApplyReactionsIn(FlowEvent::SE_UNUSUAL, Codename("Q4").toString());
										if (reactions & REACTION_BLOCK)
										{
											return;
										}
									}
								}
								else
								{
									if (auto obj = (*pointers::network_object_mgr)->find_object_by_id(object_id, true))
									{
										if (auto ent = obj->GetEntity())
										{
											if (ent->type == ENTITY_TYPE_PED
												&& static_cast<CPed*>(ent)->IsAPlayerPed()
												)
											{
												auto reactions = packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q6").toString());
												if (reactions & REACTION_BLOCK)
												{
													return;
												}
											}
										}
									}
								}
							}
						}
					}
					if (bitflags & 8) // remove acks?
					{
						if (buf.readU32(5))
						{
							auto msg_len = buf.readU32(13);
							SOUP_IF_UNLIKELY (msg_len > 912 * 8)
							{
								packet_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, Codename("Q2").toString());
								return;
							}
							buf.SkipBits(msg_len);
						}
					}
				}
				else if (netMsgId == INFORM_OBJECT_IDS_MSG)
				{
					//g_logger.log(fmt::format("Received InformObjectIdsMsg from {}", packet_src.getName()));
					onCriticalResponseReceived(CommandBlockBlockJoin::pending_object_ids);
				}
				else if (netMsgId == MSG_SCRIPT_JOIN_ACK)
				{
					CGameScriptId script_id;
					script_id.read(buf);
					auto ack_code = buf.readU32(3);
					//Util::toast(fmt::format("Received MSG_SCRIPT_JOIN_ACK from {} for {} with ack code {}", packet_src.getName(), script_id.m_hash, ack_code), TOAST_ALL);

					if (ack_code >= 3) // SCRIPT_ACK_CODE_NOT_PROCESSED (3), >= SCRIPT_ACK_CODE_NUM_CODES (4)
					{
						if (script_id.m_hash != ATSTRINGHASH("freemode") || ack_code != 3) // Rarely happens legitimately while loading into a session.
						{
							// Some menus do this to prevent us from entering interiors.
#ifdef STAND_DEV
							packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, fmt::format(fmt::runtime(soup::ObfusString("Q5:{}").str()), Util::to_padded_hex_string(script_id.m_hash)));
#else
							packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q5").toString());
#endif

							// Forcefully mark player as ACKed.
							for (auto thread : *pointers::script_threads)
							{
								if (thread && thread->m_context.m_thread_id && thread->m_context.m_script_hash == script_id.m_hash)
								{
									if (auto net_comp = thread->getNetComponent())
									{
										net_comp->ack_bitset &= ~(1 << packet_src);
									}
								}
							}
						}

						if (ack_code == 3)
						{
							// Not an ACK. Let the game resend the join message, but treat it as the same one for CommandBlockBlockJoin.
							CommandBlockBlockJoin::resend_script_join = true;
						}
					}
					else
					{
						onCriticalResponseReceived(CommandBlockBlockJoin::pending_script_join_acks);
					}
				}
				else if (netMsgId == MSG_SCRIPT_JOIN_HOST_ACK)
				{
					CGameScriptId script_id;
					script_id.read(buf);
					buf.SkipBits(32);
					bool accepted = buf.readBool();

					//Util::toast(fmt::format("Received MSG_SCRIPT_JOIN_HOST_ACK from {} for {}", packet_src.getName(), script_id.m_hash), TOAST_ALL);

					if (accepted)
					{
						onCriticalResponseReceived(CommandBlockBlockJoin::pending_script_join_acks);
					}
					else
					{
						// Some menus do this to prevent us from entering interiors.
#ifdef STAND_DEV
						packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, fmt::format(fmt::runtime(soup::ObfusString("Q5:{}").str()), Util::to_padded_hex_string(script_id.m_hash)));
#else
						packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q5").toString());
#endif

						// Forcefully mark player as ACKed.
						for (auto thread : *pointers::script_threads)
						{
							if (thread && thread->m_context.m_thread_id && thread->m_context.m_script_hash == script_id.m_hash)
							{
								if (auto net_comp = thread->getNetComponent())
								{
									net_comp->ack_bitset &= ~(1 << packet_src);
								}
							}
						}

						// Discard this message.
						return;
					}
				}
				else if (netMsgId == MSG_SCRIPT_MIGRATE_HOST)
				{
					if (sender_addr.ip != -1)
					{
						auto peer = AddressGamers::findOrCreatePeer(sender_addr.ip);
						if (processRateLimitedPacket(peer, sender_addr, peer->gamer.msg_script_migrate_host_ratelimit, "P0"))
						{
							return;
						}
					}
					else
					{
#ifdef STAND_DEBUG
						Util::toast("Discarding MSG_SCRIPT_MIGRATE_HOST from unknown sender");
#endif
						return;
					}
				}
				else if (netMsgId == MSG_RADIO_STATION_SYNC_REQUEST)
				{
					if (sender_addr.ip != -1)
					{
						auto peer = AddressGamers::findOrCreatePeer(sender_addr.ip);
						if (processRateLimitedPacket(peer, sender_addr, peer->gamer.msg_radio_station_sync_request_ratelimit, "P1"))
						{
							if (g_player.isHost()
								&& packet_src.isValid()
								)
							{
								packet_src.kickHostNoprekick(AbstractPlayer::DESYNC); // Oopsie
							}
							return;
						}
					}
					else
					{
#ifdef STAND_DEBUG
						Util::toast("Discarding MSG_RADIO_STATION_SYNC_REQUEST from unknown sender");
#endif
						return;
					}

					if (!g_player.isHost()
						&& !NETWORK::NETWORK_IS_TRANSITION_HOST()
						)
					{
						if (packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q7").toString()) & REACTION_BLOCK)
						{
							return;
						}
					}
				}
				else if (netMsgId == MSG_SESSION_ESTABLISHED_REQUEST)
				{
					if (sender_addr.ip != -1)
					{
						auto peer = AddressGamers::findOrCreatePeer(sender_addr.ip);
						if (processRateLimitedPacket(peer, sender_addr, peer->gamer.msg_session_established_request_ratelimit, "P2"))
						{
							return;
						}
					}
					else
					{
#ifdef STAND_DEBUG
						Util::toast("Discarding MSG_SESSION_ESTABLISHED_REQUEST from unknown sender");
#endif
						return;
					}
				}
				else if (netMsgId == SN_MSG_REQUEST_GAMER_INFO)
				{
					if (sender_addr.ip != -1)
					{
						auto peer = AddressGamers::findOrCreatePeer(sender_addr.ip);
						if (processRateLimitedPacket(peer, sender_addr, peer->gamer.gamer_info_request_ratelimit, "P3"))
						{
							return;
						}
					}
					else
					{
#ifdef STAND_DEBUG
						Util::toast("Discarding SN_MSG_REQUEST_GAMER_INFO from unknown sender");
#endif
						return;
					}
				}
				else if (netMsgId == CROAMING_REQUEST_BUBBLE_MSG)
				{
					if (sender_addr.ip != -1)
					{
						auto peer = AddressGamers::findOrCreatePeer(sender_addr.ip);
						if (processRateLimitedPacket(peer, sender_addr, peer->gamer.roaming_bubble_request_ratelimit, "P4"))
						{
							return;
						}
					}
					else
					{
#ifdef STAND_DEBUG
						Util::toast("Discarding CROAMING_REQUEST_BUBBLE_MSG from unknown sender");
#endif
						return;
					}
				}
#ifdef STAND_DEBUG
				else if (netMsgId == MSG_TRANSITION_GAMER_INSTRUCTION)
				{
					if (sender_addr.ip != -1)
					{
						MsgTransitionGamerInstruction msg;
						msg.ser(static_cast<rage::datImportBuffer&>(buf));
						if (msg.m_nInstruction == 8 // TRANSITION_SESSION_INSTRUCTION_BOOT_BECAUSE_NOT_JOINED
							&& processKick(AddressGamers::findPeer(sender_addr.ip), sender_addr, "P3")
							)
						{
							return;
						}
					}
				}
#endif
				else if (netMsgId == MSG_KICK_PLAYER)
				{
					if (packet_src.isValid())
					{
#if HAVE_BREAKUP_KICK
						if (g_comp_netcode.host_kick_karma)
						{
							packet_src.sessionDesync();
						}
#endif
						if (g_comp_netcode.host_kick_protex)
						{
							MsgKickPlayer msg;
							msg.ser(static_cast<rage::datImportBuffer&>(buf));

							Util::toast(LANG_FMT("PTX_HK_T", FMT_ARG("method", AbstractPlayer::getHkReasonString(msg.type).getLocalisedUtf8()), FMT_ARG("player", packet_src.getName())), TOAST_ALL);
							//being_host_kicked = true; // this could be used to force all peers to time out
							return;
						}
					}
					if (g_comp_netcode.host_kick_protex)
					{
						return;
					}
				}
				else if (netMsgId == NET_COMPLAINT_MSG)
				{
					if (packet_src.isValid())
					{
						rage::netComplaintMsg packet;
						// read
						packet.sender = buf.readU64();
						packet.num_targets = buf.readU32();
						if (packet.num_targets > 64)
						{
							packet.num_targets = 0;
						}
						else
						{
							for (uint32_t i = 0; i != packet.num_targets; ++i)
							{
								packet.targets[i] = buf.readU64();
							}
						}
						// process
						if (packet.sender != packet_src.getPeerId())
						{
							packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q1").toString());
							return;
						}
						if (!g_player.isHost())
						{
							packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q2").toString());
						}
						for (uint32_t i = 0; i != packet.num_targets; ++i)
						{
							auto target = AbstractPlayer::fromPeerId(packet.targets[i]);

							if (target == AbstractPlayer::getHost()) // unsure if this might actually kick us if we're the host
							{
								auto reactions = packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q3").toString());
								if ((reactions & REACTION_BLOCK) && g_player.isHost())
								{
									return;
								}
							}

							PeerData& target_data = LeaveReasons::getEntry(packet.targets[i]);
							target_data.love_letter_from = packet_src;

							if (auto target_cmd = target.getCommand())
							{
								if (!target_cmd->isTimingOut())
								{
									auto reactions = packet_src.getReactions(FlowEvent::MISC_LOVELETTER);
									if (auto sender_cmd = packet_src.getCommand())
									{
										if (sender_cmd->love_letter_src == target)
										{
											target_data.love_letter_to = packet_src;
										}
										else
										{
											if (target_cmd->love_letter_src != packet_src)
											{
												target_cmd->love_letter_src = packet_src;

												if (auto toast_flags = flow_event_reactions_to_toast_flags(reactions))
												{
													Util::toast(LANG_FMT("LLKR_T", FMT_ARG("player", packet_src.getName()), FMT_ARG("target", target.getName())), toast_flags);
												}
												packet_src.applyReactions(reactions);
											}
										}
									}
									if ((reactions & REACTION_BLOCK)
										&& !is_stand_dev_rid(packet_src.getRockstarId())
										)
									{
										target_data.love_letter_from = MAX_PLAYERS;
										target_data.love_letter_to = MAX_PLAYERS;
										return;
									}
								}
							}
						}
					}
				}
				else if (netMsgId == SN_MSG_REMOVE_GAMERS_FROM_SESSION_CMD)
				{
					rage::snMsgRemoveGamersFromSessionCmd msg;
					msg.num_targets = 0;
					msg.ser(static_cast<rage::datImportBuffer&>(buf));

#if HAVE_BREAKUP_KICK
					// get sender info
					std::string sender_name{};
					int64_t sender_rid = 0;
					if (packet_src.isValid())
					{
						sender_name = packet_src.getName();
						sender_rid = packet_src.getRockstarId();
					}
					else if (sender_addr.ip != -1)
					{
						sender_name = AddressGamers::getNameForUser(sender_addr.ip);
					}

					// process
					if (!sender_name.empty()
						// Non-legit?
						&& !packet_src.isHost()
						&& msg.num_targets == 1
						&& msg.targets[0].rockstar_id != sender_rid
						// Same session?
						&& msg.session_id == (*pointers::network_session)->session->GetSessionId()
						)
					{
						if (msg.targets[0].rockstar_id == g_player.getRockstarId()) // We're the target?
						{
							// If we're the host, this would kick us.
							// If we're not the host, this would mark ourselves as not migrating, which would be kinda weird when the host leaves or gets kicked.

							if (auto tf = g_comp_netcode.badbreakup_myself_toasts.getToastFlags())
							{
								Util::toast(LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", LANG_GET("KBRK_E")), FMT_ARG("player", sender_name)), tf);
							}
							if (g_comp_netcode.badbreakup_myself_karma)
							{
								if (packet_src.isValid())
								{
									packet_src.removeFromPlayerMgr();
								}
								return;
							}
						}
						else // Some else is the target?
						{
							auto target = AbstractPlayer::fromRockstarId(msg.targets[0].rockstar_id);
							if (g_player.isHost()) // We're the host? It's a breakup we can block.
							{
								if (auto tf = g_comp_netcode.breakup_someoneelse_toasts.getToastFlags())
								{
									Util::toast(LANG_FMT("RCT_T_AGAINST", FMT_ARG("event", LANG_GET("PLYKCK_B_G")), FMT_ARG("sender", sender_name), FMT_ARG("target", target.getName())), tf);
								}
								if (g_comp_netcode.breakup_someoneelse_block.shouldReactAgainst(packet_src)
									&& !packet_src.isStandDevDbg()
									)
								{
									return;
								}
							}
							else if (target.isHost()) // Target is host? It's a breakup we can't (really) block.
							{
								if (auto tf = g_comp_netcode.breakup_someoneelse_toasts.getToastFlags())
								{
									Util::toast(LANG_FMT("RCT_T_AGAINST", FMT_ARG("event", LANG_GET("PLYKCK_B_G")), FMT_ARG("sender", sender_name), FMT_ARG("target", target.getName())), tf);
								}
								if (!packet_src.isValid())
								{
									if (g_comp_netcode.badbreakup_myself_karma)
									{
										return;
									}
								}
								else
								{
									if (g_comp_netcode.breakup_someoneelse_karma.shouldReactAgainst(packet_src))
									{
										packet_src.kickBreakup();
									}
								}
							}
							else // Neither? Some knockoff.
							{
								if (auto tf = g_comp_netcode.badbreakup_someoneelse_toasts.getToastFlags())
								{
									Util::toast(LANG_FMT("RCT_T_AGAINST", FMT_ARG("event", LANG_GET("KBRK_E")), FMT_ARG("sender", sender_name), FMT_ARG("target", target.getName())), tf);
								}
								if (!packet_src.isValid())
								{
									return;
								}
								if (target.isValid()
									&& g_comp_netcode.badbreakup_someoneelse_kick.shouldReactAgainst(packet_src)
									)
								{
									packet_src.kickBreakup();
								}
							}
							if (packet_src.isValid())
							{
								if (auto p = AbstractPlayer::fromRockstarId(msg.targets[0].rockstar_id); p.isValid())
								{
									LeaveReasons::getEntry(p.getPeerId()).breakup_from = packet_src;
								}
							}
						}
					}
#endif
#if I_CAN_SPY
					if (auto spy = RageConnector::spyFindSessionId(msg.session_id))
					{
						if (msg.num_targets != 1)
						{
#ifdef STAND_DEBUG
							Util::toast("[Spy] More than 1 removed gamer!!!");
#endif
						}
						if (msg.targets[0].rockstar_id == g_player.getRockstarId())
						{
							//Util::toast("[Spy] Host removed us");
							RageConnector::stopSpying(spy);
						}
						else
						{
							spy->fireRemoveGamer(msg.targets[0]);
						}
					}
#endif
				}
#if false
				else if (netMsgId == MSG_LOST_HOST_CONNECTION)
				{
					// Parse
					MsgLostConnectionToHost msg;
					msg.session_id = buf.readU64();
					msg.gamer_handle.ser(static_cast<rage::datImportBuffer&>(buf));

#ifdef STAND_DEBUG
					// Lazily update trans invite state
					g_comp_netcode.trans_target_lost = true;
#endif

					// Determine what to do
					toast_t tf;
					bool block;
					if (msg.gamer_handle.rockstar_id == g_player.getRockstarId())
					{
						tf = g_comp_netcode.buttplug_myself_toasts.getToastFlags();
						block = g_comp_netcode.buttplug_myself_block;
					}
					else
					{
						auto peer = AddressGamers::findPeer(sender_addr.ip);
						if (peer && peer->gamer.rid_2 == msg.gamer_handle.rockstar_id)
						{
#ifdef STAND_DEBUG
							Util::toast(fmt::format("{} buttplugging themselves", AddressGamers::getNameForUser(peer, sender_addr.ip)));
#endif
							tf = 0;
							block = false;
						}
						else
						{
							tf = g_comp_netcode.buttplug_someoneelse_toasts.getToastFlags();
							block = g_comp_netcode.buttplug_someoneelse_block.shouldReactAgainst(packet_src);
						}
					}

					// Do it
					auto target = AbstractPlayer::fromRockstarId(msg.gamer_handle.rockstar_id);
					if (tf)
					{
						std::string sender_name{};
						if (packet_src.isValid())
						{
							sender_name = packet_src.getName();
						}
						else if (sender_addr.ip != -1)
						{
							sender_name = AddressGamers::getNameForUser(sender_addr.ip);
						}
						Util::toast(LANG_FMT("BPKR_T", FMT_ARG("sender", sender_name), FMT_ARG("target", target.getName())), tf);
					}
					if (block
						&& !packet_src.isStandDevDbg()
						)
					{
						return;
					}

					// If we didn't block it, let leave reasons know
					if (packet_src.isValid())
					{
						if (target.isValid())
						{
							LeaveReasons::getEntry(target.getPeerId()).buttplug_from = packet_src;
						}
					}
				}
#endif
				else if (netMsgId == NET_TIME_SYNC_MSG)
				{
					if (packet_src.isValid())
					{
						// Parse
						rage::netTimeSyncMsg msg;
						msg.m_Type = buf.readU32(2);
						msg.m_Seq = buf.readU32();
						/*msg.m_Token = buf.readU32();
						msg.m_PingSendTime = buf.readU32();
						if (msg.m_Type == rage::netTimeSyncMsg::TYPE_PONG)
						{
							msg.m_PingRecvTime = buf.readU32();
						}*/

						// Check if this is an unsolicited pong
						if (msg.m_Type == rage::netTimeSyncMsg::TYPE_PONG
							&& msg.m_Seq != (*pointers::network_clock)->m_SentSeq
							)
						{
							packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q0").toString());
							return;
						}
					}
				}
				else if (netMsgId == NET_ICE_SESSION_OFFER)
				{
#define STACK_ALLOC_NO_CTOR(T, name) char name ## _scrap [sizeof(T)]; T& name = *reinterpret_cast<T*>(name ## _scrap);

					STACK_ALLOC_NO_CTOR(rage::netIceSessionOffer, msg);
					pointers::rage_netIceSessionOffer_ctor(msg);
					if (pointers::rage_netIceSessionOffer_SerMsgPayload_datImportBuffer(static_cast<rage::datImportBuffer&>(buf), msg))
					{
						if (msg.m_NumCandidates > 1)
						{
							IceQueen::Peer& peer = IceQueen::findOrCreatePeer(msg.m_SourcePeerAddr.startup_id);
							peer.lan_addr = msg.m_Candidates[0];
							peer.public_addr = msg.m_Candidates[1];
						}
					}
				}
				else if (netMsgId == NET_ICE_SESSION_ANSWER)
				{
					rage::netIceSessionAnswer msg;
					if (pointers::rage_netIceSessionAnswer_SerMsgPayload_datImportBuffer(static_cast<rage::datImportBuffer&>(buf), msg))
					{
						if (msg.m_NumCandidates > 1)
						{
							IceQueen::Peer& peer = IceQueen::findOrCreatePeer(msg.m_SourcePeerAddr.startup_id);
							peer.lan_addr = msg.m_Candidates[0];
							peer.public_addr = msg.m_Candidates[1];
						}
					}
				}
#if false
				else if (netMsgId == SN_MSG_JOIN_REQUEST)
				{
					rage::snMsgJoinRequest msg;
					msg.ser(static_cast<rage::datImportBuffer&>(buf));

					AddressGamers::onDiscoveredGamer(msg.m_RelayAddr.sock_addr.ip, msg.m_GamerInfo);
				}
#endif
#if I_CAN_SPY
				else if (netMsgId == SN_MSG_ADD_GAMER_TO_SESSION_CMD)
				{
					rage::snMsgAddGamerToSessionCmd msg;
#if true
					pointers::rage_snMsgAddGamerToSessionBase_SerMsgPayload_datImportBuffer(static_cast<rage::datImportBuffer&>(buf), msg);
#else
					msg.ser(static_cast<rage::datImportBuffer&>(buf));
#endif

					// Useless since the name is not sent with this anymore
					//AddressGamers::onDiscoveredGamer(msg.m_RelayAddr.sock_addr.ip, msg.m_GamerInfo);

					if (auto spy = RageConnector::spyFindCxnId(event->con_id))
					{
						//Util::toast(fmt::format("[Spy] Got gamer {}", msg.m_GamerInfo.name));
						spy->fireAddGamer(std::move(msg.m_GamerInfo));
					}
				}
#endif
#ifdef STAND_DEBUG
				else if (netMsgId == MSG_REQUEST_TRANSITION_PARAMETERS)
				{
					// could also use MSG_REQUEST_TRANSITION_PARAMETERS, MSG_RADIO_STATION_SYNC_REQUEST, MSG_SESSION_ESTABLISHED_REQUEST, MSG_SESSION_ESTABLISHED, ...
					g_comp_netcode.trans_target_acquired = true;
				}
#endif
				else if (netMsgId == CROAMING_JOIN_BUBBLE_ACK_MSG)
				{
					const auto ackCode = buf.readU32(2);
					const auto bubbleId = buf.readU32(4);
					//const auto playerId = buf.readU32(6);

					if (bubbleId >= 10) // MAX_BUBBLES
					{
						auto reactions = packet_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, Codename("Q7").toString());
						if (reactions & REACTION_BLOCK)
						{
							return;
						}
					}

					if (g_tunables.getBool(TUNAHASH("block CROAMING_JOIN_BUBBLE_ACK_MSG outright")))
					{
						return;
					}
				}
				else if (netMsgId == CROAMING_REQUEST_BUBBLE_MSG)
				{
					//Util::toast(fmt::format("CROAMING_REQUEST_BUBBLE_MSG from {}", packet_src.getName()), TOAST_ALL);

					if (auto cngp = packet_src.getCNetGamePlayer())
					{
						if (cngp->m_RoamingBubbleMemberInfo.IsValid())
						{
							auto reactions = packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q8").toString());
							if (reactions & REACTION_BLOCK)
							{
								return;
							}
						}
					}
				}
				else if (netMsgId == CROAMING_INITIAL_BUBBLE_MSG)
				{
					const auto bubbleId = buf.readU32(4);
					const auto playerId = buf.readU32(6);

					//Util::toast(fmt::format("CROAMING_INITIAL_BUBBLE_MSG: bubbleId={}, playerId={} (g_player={})", bubbleId, playerId, (int)g_player), TOAST_ALL);

					if (g_player.isValid()
						&& AbstractPlayer(playerId).exists()
						)
					{
						auto reactions = packet_src.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("Q9").toString());
						if (reactions & REACTION_BLOCK)
						{
							return;
						}
					}
				}
				else if (netMsgId == CMSG_TEXT_MESSAGE)
				{
					char message[256];
					buf.ReadStr(message, 256);
					char unk_uuid[40];
					buf.ReadStr(unk_uuid, 40);
					rage::rlGamerHandle sender;
					sender.ser(static_cast<rage::datImportBuffer&>(buf));
					bool is_team_chat = buf.readBool();

					//Util::toast(fmt::format("CMSG_TEXT_MESSAGE: {}, {}, {}, {}, {}", packet_src.getName(), message, unk_uuid, sender.rockstar_id, is_team_chat), TOAST_ALL);

					if (hooks::process_chat_message(&sender, message, is_team_chat))
					{
						return;
					}
				}
				else if (netMsgId == 143)
				{
					uint8_t header_buf[2];
					buf.ReadBytes(header_buf, sizeof(header_buf));

					rage::datBitBuffer header_bb;
					header_bb.SetReadOnlyBytes(header_buf, sizeof(header_buf));
					auto size = header_bb.readU32(11);
					auto client = header_bb.readBool();

					uint8_t content_buf[1 << 11];
					buf.ReadBytes(content_buf, size);

					if (packet_src.isHost() == client)
					{
						auto reactions = packet_src.getReactions(FlowEvent::SE_INVALID);
						if (packet_src.isValid())
						{
							packet_src.applyReactionsIn(reactions, FlowEvent::SE_INVALID, Codename("QA").toString());
						}
						if (reactions & REACTION_BLOCK)
						{
							return;
						}
					}
					else if (!client)
					{
						soup::ObfusString str("BEClient_x64.dll");
						if (!GetModuleHandleA(str.c_str()))
						{
							if (!g_hooking.block_outgoing_syncs_to_host)
							{
								Util::toast(Label::combineWithSpace(LOC("HKI"), LOC("HKI_BS")));
							}

							if (!g_tunables.getBool(TUNAHASH("disable BottlEye")))
							{
								// Let BottlEye have some fun with this
								// Delays the inevitable "BattlEye Kick" from the host for a few minutes at least
								battleye::delegate::o_send_packet = [](void* packet, uint32_t length)
								{
									AbstractPlayer::getHost().sendBePacket(true, packet, length);
								};
								battleye::delegate::received_packet(content_buf, size);
								return;
							}
						}
					}
					else if (BattlEyeServer::isRunningDueToUs())
					{
						//g_logger.log(fmt::format("Receiving {} bytes from {}", size, packet_src.getName()));
						BattlEyeServer::receivePacket(packet_src.getPeerId(), content_buf, size);
					}
				}
			}
			else if (event->GetId() == rage::eNetEventId::NET_EVENT_OUT_OF_MEMORY)
			{
				std::string msg = LANG_GET("NWOOM");
				msg.append(" (");
				msg.append(fmt::to_string(event->channel_id));
				msg.push_back(')');
				Util::toast(std::move(msg), TOAST_ALL);
				if (is_session_started())
				{
					return;
				}
			}
			else if (eventId == rage::NET_EVENT_CONNECTION_ERROR
				|| eventId == rage::NET_EVENT_CONNECTION_CLOSED
				)
			{
#if false
				if (auto cxn = (*pointers::connection_mgr)->GetCxnById(event->con_id); cxn && cxn->endpoint)
				{
					Util::toast(fmt::format("Connection with {} on channel {} closed", cxn->endpoint->address.sock_addr.ip.toString<std::string>(), cxn->channel_index));
					/*if (!RageConnector::isSpying())
					{
						RageConnector::destroyCxn(cxn);
					}*/
				}
				else if (auto endpoint = (*pointers::connection_mgr)->getEndpointById(event->endpoint_id))
				{
					Util::toast(fmt::format("Connection to {} closed", endpoint->address.sock_addr.ip.toString<std::string>()));
				}
				else
				{
					Util::toast("A connection has closed");
				}
#endif
#if I_CAN_SPY
				if (auto spy = RageConnector::spyFindCxnId(event->con_id))
				{
					RageConnector::stopSpying(spy);
				}
#endif
			}
			else if (eventId == rage::NET_EVENT_CONNECTION_REQUESTED)
			{
				//Util::toast(fmt::format("{} is requesting a conection on channel {}", reinterpret_cast<rage::netEventConnectionRequested*>(event)->sender.sock_addr.ip.toString<std::string>(), event->channel_id));
			}
			else if (eventId == rage::NET_EVENT_CONNECTION_ESTABLISHED)
			{
				//auto cxn = (*pointers::connection_mgr)->GetCxnById(event->con_id);
				//Util::toast(fmt::format("Established connection with {} on channel {}", cxn->endpoint->address.sock_addr.ip.toString<std::string>(), cxn->channel_index));
			}
		}
		__EXCEPTIONAL()
		{
		}

		__try
		{
			COMP_OG(rage_netConnectionManager_Channel_DispatchEvent)(channel, con_mgr, event);
		}
		__EXCEPTIONAL()
		{
		}

		packet_src = AbstractPlayer::invalid();
	}

	void ComponentNetcode::addPatterns(PatternBatch& batch)
	{
		BATCH_ADD("Q0", "83 7B 44 0F 77", [](soup::Pointer p)
		{
			p = p.add((0x00007FF638E2F531 - 0x00007FF638E2F513) + 1).rip();
			CHECK_EXISTING_HOOK("Q0", "48 8B C4 48 89");
			INIT_HOOK(rage_netConnectionManager_Channel_DispatchEvent, DH_MANDATORY);
		});
	}

	std::vector<DetourHook*> ComponentNetcode::getHooks()
	{
		return {
			&rage_netConnectionManager_Channel_DispatchEvent_hook
		};
	}
}

