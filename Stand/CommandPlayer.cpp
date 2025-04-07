#include "CommandPlayer.hpp"

#include <fmt/core.h>

#include <soup/bitutil.hpp>
#include <soup/country_names.hpp>
#include <soup/JsonObject.hpp>
#include <soup/netIntel.hpp>
#include <soup/unicode.hpp>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "AbstractPlayer.hpp"
#include "AddressGamers.hpp"
#include "Auth.hpp"
#include "BattlEyeServer.hpp"
#include "cidr.hpp"
#include "ColoadMgr.hpp"
#include "CNetGamePlayer.hpp"
#include "CNetworkSession.hpp"
#include "CommandHistoricPlayer.hpp"
#include "CommandPlayerClassifier.hpp"
#include "CommandPlayerClassifierDetection.hpp"
#include "CommandPlayerIntel.hpp"
#include "CommandToggle.hpp"
#include "CPedIntelligence.hpp"
#include "CPlayerInfo.hpp"
#include "CTaskJump.hpp"
#include "DirectPacket.hpp"
#include "eTaskType.hpp"
#include "evtFlowEventDoneEvent.hpp"
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "FiberPool.hpp"
#include "fmt_arg.hpp"
#include "get_current_time_millis.hpp"
#include "gta_net_object_mgr.hpp"
#include "gta_ped.hpp"
#include "get_seconds_since_unix_epoch.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "is_rid.hpp"
#include "is_session.hpp"
#include "joaatToString.hpp"
#include "LeaveReasons.hpp"
#include "FileLogger.hpp"
#include "ManagedTunables.hpp"
#include "name_validation.hpp"
#include "natives.hpp"
#include "netIpAddress.hpp"
#include "netSocketAddress.hpp"
#include "NuancedEventWithExtraData.hpp"
#include "PlayerHistory.hpp"
#include "PlayerListSort.hpp"
#include "PlayerProviderSingle.hpp"
#include "players_interface.hpp"
#include "Pong.hpp"
#include "pointers.hpp"
#include "RemoteGamer.hpp"
#include "Renderer.hpp"
#include "reversible_scramble.hpp"
#include "rlPc.hpp"
#include "rlRosCredentials.hpp"
#include "rockstar_admins.hpp"
#include "Sanity.hpp"
#include "Script.hpp"
#include "script_events.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"
#include "SessionSpoofing.hpp"
#include "StringUtils.hpp"
//#include "Tunables.hpp"
#include "UnicodePrivateUse.hpp"
#include "Util.hpp"

// When joining a new session,
// 1. we send the session id to auth,
// 2. auth lets us know about possible stand users in our session,
// 3. then (if we have SUI enabled or auth says remote is devdbg) we send DP_HANDSHAKE.
//
// When we receive a DP_HANDSHAKE, we reply with DP_I_GOT_YOU if we have SUI enabled or DPFLAG_DEVDBG is set.
// Note that we don't have a trusted way to verify that the remote is devdg since we only get auth info when first joining.
// 
// Since we make sure to only send direct packets when SUI is enabled (or remote is devdbg),
// we can simply detect a Stand user due to their sending of a direct packet.
// To ensure that remote is actually a Stand user and not just someone emulating direct packets,
// the direct packet header includes a magic value provided by auth.

#define DEBUG_SUI_HANDSHAKE false

#ifdef STAND_DEBUG
#define SUI_DETECT_WHEN_OFF true
#else
#define SUI_DETECT_WHEN_OFF false
#endif

#define SUI_DO_FORCE_DETECT false
#define SUI_DO_RESET_DETECT false

namespace Stand
{
	char32_t CommandPlayer::flag_chars[/*FLAG_COUNT*/] = {
		0, // FLAG_SELF
		'F', // FLAG_FRIEND
		'C', // FLAG_CREW
		'B', // FLAG_TIMEOUT
		'M', // FLAG_MOD
#if HAS_MODAMN
		'?', // FLAG_MODAMN
#endif
		UnicodePrivateUse::rs[0], // FLAG_AMN
		'-', // FLAG_LIKELY_MODDER
		'!', // FLAG_ATTACKED
		'H', // FLAG_HOST
		0, // FLAG_NEXT_HOST
		'S', // FLAG_SCRIPT_HOST
		'G', // FLAG_GOD
		'V', // FLAG_VEHGOD
		0, // FLAG_INVEH
		'R', // FLAG_RC
		'O', // FLAG_OTR
		'I', // FLAG_INT
		'P', // FLAG_SPEC
		'T', // FLAG_TYPING
		'D', // FLAG_DEAD
		'/', // FLAG_INVISIBLE
		'#', // FLAG_VPN
		'W', // FLAG_ORG_MEMBER
		0, // FLAG_STAND_USER
		0 // FLAG_GEOIP
	};

#define INIT_RATELIMIT(name, window_duration_ms, max_window_requests) name(g_tunables.getInt(TUNAHASH(#name ".window_duration_ms"), window_duration_ms), static_cast<unsigned int>(g_tunables.getInt(TUNAHASH(#name ".max_window_requests"), max_window_requests)))

	CommandPlayer::CommandPlayer(CommandList* const parent, const compactplayer_t p, bool notify_join)
		: CommandListPlayer(parent, &_player_provider, LIT(AbstractPlayer(p).getName()), {}, CMDFLAGS_LIST_CONCEALER | CMDFLAG_TEMPORARY),
		_player_provider(p),
		name(menu_name.getLiteralUtf8()),
		joined(!g_gui.players_discovered || !AbstractPlayer::is_freemode || is_session_transition_active(true) || (ScriptGlobal(GLOBAL_PLAYERS_JOINED_BITSET).get<int>() & (1 << p))),
		discovery(get_current_time_millis())
		//,INIT_RATELIMIT(veh_appearance_ratelimit, 5000, 40)
	{
		// Init vars
		const bool is_self = (p == g_player);
		if (is_self)
		{
			did_rid_spoof_lookup = true;
			flags |= 1 << FLAG_SELF;
		}
		AbstractPlayer ap(p);
		peerid = ap.getPeerId();
		rid = ap.getRockstarId();
		updateCachedVars();
		has_silentchat_chance = is_session_transition_active(true);

		// Filtering Name & Setting Command Name
		streamer_spoof_is_off = (ap.getOverrideName() == nullptr);
		std::string session_name = ap.getSessionName();
		std::string filtered_name = filter_name_pretty(session_name);
		if (session_name != filtered_name)
		{
			command_names.emplace_back(std::move(CommandName(1, CMDNAME('p')).append(utf8ToCmdName(filtered_name))));
		}
		else
		{
			command_names.emplace_back(std::move(CommandName(1, CMDNAME('p')).append(menu_name.getLiteralForCommandName())));
		}
		AbstractPlayer::redundant_names.at(p) = name;
		StringUtils::simplifyCommandName(command_names.at(0));

		if (is_self || g_gui.doesRootStateAllowPlayerList())
		{
			populate(p);
		}

		if (g_gui.root_state <= GUI_FREE)
		{
			if (!is_self)
			{
				fastTrackRidSessionMismatch();
			}
			return;
		}

		if (g_gui.unlock_recover_state != 2)
		{
			Sanity::discovered_other_player_non_free_without_recover_state = true;
		}

		if (!is_self)
		{
			if (BattlEyeServer::isRunningDueToUs())
			{
				BattlEyeServer::addPlayer(peerid, rid, session_name.c_str());
			}

			// Find in history
#if ASSUME_NO_RID_SPOOFING
			historic_player = PlayerHistory::findRID(rid);
#else
			rage::rlGamerId gamerid = ap.getGamerId();
			historic_player = PlayerHistory::findGamerId(gamerid);
			historic_player_rid = PlayerHistory::findRID(rid);
#endif
			if (historic_player != nullptr)
			{
				processRediscovery(ap);
				PlayerHistory::save();
#if !ASSUME_NO_RID_SPOOFING
				processed_rediscovery = true;
#endif
			}
#if !ASSUME_NO_RID_SPOOFING
			else if (gamerid.isCongruentWithName(session_name))
			{
				historic_player = PlayerHistory::findName(session_name);
				if (historic_player != nullptr)
				{
					processRediscovery(ap);
					PlayerHistory::save();
					processed_rediscovery = true;
				}
			}
			if (historic_player == nullptr)
			{
				const uint32_t mac_id = ap.getMacId();
				bool can_rediscover_by_mac = !gamerid.isCongruentWithNameAndMacId(session_name, mac_id);
				if (!can_rediscover_by_mac
					&& historic_player
					)
				{
					rage::rlGamerId historic_gamer_id = historic_player->getGamerId();
					if (historic_gamer_id.isValid()
						&& historic_gamer_id.isCongruentWithName(historic_player->name)
						)
					{
						can_rediscover_by_mac = true;
					}
				}
				if (can_rediscover_by_mac
					&& PlayerHistory::isUniqueMacAddressHash(mac_id)
					)
				{
					historic_player = PlayerHistory::findMacId(mac_id);
					if (historic_player != nullptr)
					{
#ifdef STAND_DEBUG
						g_logger.log(fmt::format("Rediscovered {} by mac", ap.getName()));
#endif
						processRediscovery(ap);
						PlayerHistory::save();
						processed_rediscovery = true;
					}
				}
			}
			if (historic_player_rid == historic_player)
			{
				historic_player_rid = nullptr;
			}
#endif

			// Join event
			if (notify_join)
			{
				floweventreaction_t reactions = ap.getReactions(FlowEvent::MISC_JOIN);
				if (historic_player != nullptr)
				{
#if HAVE_BREAKUP_KICK
					reactions |= (historic_player->join_reactions & ~REACTION_KICK); // exclude "Block Join" reaction
#else
					reactions |= historic_player->join_reactions;
#endif
				}
				if (auto toast_flags = flow_event_reactions_to_toast_flags(reactions))
				{
					Util::toast(LANG_FMT("JOINT", ap.getGamerInfo()->toString()), std::move(toast_flags));
				}
				if (!g_player.isHost()
					&& hooks::process_join(ap.getGamerInfo(), false)
					)
				{
					reactions |= REACTION_KICK;
				}
				applyReactions(reactions);
			}
		}

		// Classification based on gamer info
		{
			const auto prev_classification = classifier.classify();

			for (auto& event : ap.getGamerInfo()->checkDetections(is_self))
			{
				if (event.isPreemptive())
				{
					addOrUpdateDetection(event, event.getName());
				}
				else
				{
					applyReactions(ap.getReactions(event.type), std::move(event), true);
				}
			}

			if (auto cngp = ap.getCNetGamePlayer())
			{
				if (cngp->m_NatType == 0
					|| (cngp->m_NatType != ap.getGamerInfo()->peer.address.nat_type && ap.getGamerInfo()->peer.address.nat_type != 0)
					)
				{
					if (auto host_cngp = AbstractPlayer::getHost().getCNetGamePlayer())
					{
						// Only do this detection if we're the host or this player is the host,
						// to rule out the current host spoofing this for the whole lobby.
						if (cngp == host_cngp
							|| g_player.isHost()
							)
						{
							applyReactions(ap.getReactions(FlowEvent::MOD_LLLUBE), NuancedEvent(FlowEvent::MOD_LLLUBE), true);
						}
					}
				}
			}

			PlayerClassifier::Classification classification;
			if (auto reactions = checkClassificationUpdate(classification, prev_classification))
			{
				notifyClassificationUpdate(classification, flow_event_reactions_to_toast_flags(reactions), reactions);
				applyReactions(reactions);
			}
		}

		if (!is_self)
		{
			// Save to history
			if (historic_player == nullptr
#if !ASSUME_NO_RID_SPOOFING
				&& historic_player_rid == nullptr
#endif
				)
			{
#if ASSUME_NO_RID_SPOOFING
				uint8_t flags = 0;
#else
				uint8_t flags = HP_DISTRUST_RID;
#endif
				historic_player = PlayerHistory::insert(rid, ap.getMacId(), std::move(filtered_name), get_seconds_since_unix_epoch(), flags);
#if !ASSUME_NO_RID_SPOOFING
				processed_rediscovery = true;
#endif
			}

			// historic_player is now set up correctly for concludeRidChecks
			fastTrackRidSessionMismatch();

			if (auto ip = ap.getAddress().ip; ip != -1)
			{
#if true
				// Register with AddressGamers
				AddressGamers::onDiscoveredGamer(ip, *ap.getGamerInfo());
#else
				// Register with AddressGamers if it's possible ComponentNetcode didn't have a chance to
				if (!g_gui.players_discovered)
				{
					AddressGamers::onDiscoveredGamer(ip, *ap.getGamerInfo());
				}
#endif
			}

			// Register physical index with leave reasons
			LeaveReasons::getEntry(ap.getPeerId()).physical = p;
		}
	}

	void CommandPlayer::fastTrackRidSessionMismatch()
	{
#if !ASSUME_NO_RID_SPOOFING
		const bool is_my_rid = (rid == pointers::rlPresence_GamerPresences_0_GamerInfo->getHandle().rockstar_id);
		if (is_my_rid)
		{
			getAndApplyReactions(getPlayer(), FlowEvent::MOD_SPOOFEDRIDHARD);
			concludeRidChecks(0);
		}
		else if (!is_rid_valid(rid))
		{
			concludeRidChecks(0);
		}
#endif
	}

	void CommandPlayer::onLeave()
	{
#ifdef STAND_DEV
		if (historic_player == nullptr
			&& !isSelf()
			)
		{
			g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("{} left without being added to player history. This is fine, I guess.").str()), getPlayerName()));
		}
#endif
		getPlayer().clearOverrideName();

		if (BattlEyeServer::isRunningDueToUs())
		{
			BattlEyeServer::removePlayer(peerid);
		}
	}

	flowevent_t CommandPlayer::getAvailableCustomFlowEvent() const noexcept
	{
		flowevent_t i = FlowEvent::_CUSTOM_BEGIN;
		for (; i != FlowEvent::_CUSTOM_END; ++i)
		{
			if (!classifier.hasEvent(i))
			{
				break;
			}
		}
		return i;
	}

	bool CommandPlayer::isStandUser() const noexcept
	{
		return isMarkedAsStandUser();
	}

	bool CommandPlayer::isMarkedAsStandUser() const noexcept
	{
		return (flags >> FLAG_STAND_USER) & 1;
	}

	bool CommandPlayer::hasStandUserHandshakeFinished() const noexcept
	{
		return isStandUser()
			|| (attempted_stand_user_handshake
				? GET_MILLIS_SINCE(sent_stand_user_handshake_at) > 3000
				: (!joined && GET_MILLIS_SINCE(discovery) > 15000)
				)
			;
	}

	bool CommandPlayer::isTimingOut(time_t delay) const noexcept
	{
		return GET_MILLIS_SINCE(last_packet) > delay && last_packet != 0;
	}

	std::wstring CommandPlayer::getCommandSyntax() const
	{
		return getCommandSyntaxImpl(std::move(std::wstring(L"p ").append(cmdNameToUtf16(getCommandNamesSuffix()))));
	}

	void CommandPlayer::onClick(Click& click)
	{
		if (!queued_for_deletion)
		{
			CommandList::onClick(click);

			if (on_click_enable_spectate == 1)
			{
				getSpectateNinjaCommand()->setStateBool(click, true);
			}
			else if (on_click_enable_spectate == 2)
			{
				resolveChildByMenuName(LOC("PLYSPEC"))->as<CommandList>()->children.at(1)->as<CommandToggleNoCorrelation>()->setStateBool(click, true);
			}
		}
	}

	CommandToggleNoCorrelation* CommandPlayer::getSpectateNinjaCommand() const
	{
		return resolveChildByMenuName(LOC("PLYSPEC"))->as<CommandList>()->children.at(0)->as<CommandToggleNoCorrelation>();
	}

	void CommandPlayer::onBack(ThreadContext tc)
	{
		if (on_back_disable_spectate)
		{
			for (const auto& method : resolveChildByMenuName(LOC("PLYSPEC"))->as<CommandList>()->children)
			{
				if (method->as<CommandToggleNoCorrelation>()->m_on)
				{
					Click click(CLICK_AUTO, tc);
					method->as<CommandToggleNoCorrelation>()->setStateBool(click, false);
				}
			}
		}
	}

	bool CommandPlayer::isSelf() const noexcept
	{
		return (flags & (1 << FLAG_SELF));
	}

	AbstractPlayer CommandPlayer::getPlayer() const
	{
		return AbstractPlayer(((const PlayerProviderSingle*)this->pp)->p);
	}

	std::string CommandPlayer::getPlayerName() const
	{
		return getPlayer().getName();
	}

	CommandName CommandPlayer::getCommandNamesSuffix() const
	{
		return this->command_names.cbegin()->substr(1);
	}

	void CommandPlayer::concludeRidChecks(int64_t real_rid)
	{
#if !ASSUME_NO_RID_SPOOFING
		this->real_rid = real_rid;
#endif
		this->did_rid_spoof_lookup = true;

		if (g_gui.root_state <= GUI_FREE)
		{
#if FREE_PROTECTIONS
			if (g_gui.root_state != GUI_FREEONLINE)
#endif
			{
				Sanity::discovered_other_player_in_invalid_root_state = true;
			}
			return;
		}

#if ASSUME_NO_RID_SPOOFING
		if (real_rid != 0)
		{
			checkRockstarAdmin(real_rid);
		}
#else
		auto p = getPlayer();

		if (real_rid != 0)
		{
			checkRockstarAdmin(real_rid);

			if (historic_player == nullptr) // historic_player can only remain nullptr if historic_player_rid != nullptr
			{
				historic_player = historic_player_rid;
				if (historic_player->flags & HP_DISTRUST_RID)
				{
					historic_player->flags &= ~HP_DISTRUST_RID;
					PlayerHistory::save();
				}
			}
			else if (historic_player_rid == nullptr) // not seen this rid in another entry, we can update rid in this one
			{
				if (historic_player->rid != real_rid
					|| (historic_player->flags & HP_DISTRUST_RID)
					)
				{
					historic_player->rid = real_rid;
					historic_player->flags &= ~HP_DISTRUST_RID;
					PlayerHistory::save();
				}
			}
			else // we have seen this rid in another entry, we have to resolve this conflict
			{
				if (historic_player_rid->flags & HP_DISTRUST_RID)
				{
					// likely someone spoofing as this person
					if (historic_player_rid->getGamerId().isValid())
					{
						// their entry can continue to exist without the rid
						historic_player_rid->rid = 0;
						historic_player_rid->flags &= ~HP_DISTRUST_RID;
#ifdef STAND_DEBUG
						g_logger.log(fmt::format("{} has conflicting entry {}, likely someone spoofing, we have their gamerid, so removing rid", p.getName(), historic_player_rid->name));
#endif
					}
					else
					{
						// their entry is useless
#ifdef STAND_DEBUG
						g_logger.log(fmt::format("{} has conflicting entry {}, has no gamer id, yeeting it", p.getName(), historic_player_rid->name));
#endif
						PlayerHistory::erase(historic_player_rid);
						historic_player_rid = nullptr;
					}
				}
				else
				{
					// likely same person, we will just replace the entry
#ifdef STAND_DEBUG
					g_logger.log(fmt::format("{} has conflicting entry {}, assimilating", historic_player_rid->name, historic_player->name));
#endif
					historic_player_rid->assimilate(*historic_player);
					historic_player_rid->flags &= ~HP_DISTRUST_RID;
					PlayerHistory::erase(historic_player);
					historic_player = historic_player_rid;
				}
				PlayerHistory::save();
			}
		}
		else // spoofed rid
		{
			if (historic_player == nullptr) // historic_player can only remain nullptr if historic_player_rid != nullptr
			{
				// first time meeting this person who is spoofing to a known rid
				auto p = getPlayer();
				historic_player = PlayerHistory::insert(p.getScrambledGamerId(), 0, p.getMacId(), filter_name_pretty(p.getSessionName()), get_seconds_since_unix_epoch(), 0);
				processed_rediscovery = true;
			}
		}

		if (!processed_rediscovery)
		{
			processRediscovery(p);
			PlayerHistory::save();
		}
#endif
	}

	void CommandPlayer::checkRockstarAdmin(int64_t real_rid) const
	{
		auto p = getPlayer();

		if (!g_auth.hasApiCredentials())
		{
			Sanity::discovered_other_player_without_api_credentials = true;
		}
		else if (g_auth.isLatestVersion())
		{
			auto ip = p.getAddress().ip;
			if (ip.value != -1)
			{
				if (CidrSupernets::take_two.contains(ip.value))
				{
					if (!is_admin_rid(rid))
					{
						soup::JsonObject obj{};
						obj.add("r", real_rid);
						obj.add("n", p.getSessionName());
						obj.add("i", ip.operator std::string());
						g_auth.reportEvent("A1", obj.encode());
					}
				}
				else
				{
					if (is_admin_rid(rid))
					{
						soup::JsonObject obj{};
						obj.add("r", real_rid);
						obj.add("n", p.getSessionName());
						obj.add("i", ip.operator std::string());
						g_auth.reportEvent("A0", obj.encode());
					}
				}
			}
		}
	}

	void CommandPlayer::processRediscovery(AbstractPlayer p) const
	{
		auto session_name = p.getSessionName();
		auto filtered_name = filter_name_pretty(session_name);

		if (historic_player->name != filtered_name)
		{
			Util::toast(LANG_FMT("NMECHNG", FMT_ARG("newname", filtered_name), FMT_ARG("oldname", historic_player->name)), streamer_spoof_is_off ? TOAST_ALL : TOAST_LOGGER);
		}

		if (historic_player->name != session_name
			&& filtered_name == session_name
			)
		{
			EXCEPTIONAL_LOCK(PlayerHistory::mtx)
			historic_player->name = session_name;
			historic_player->flags &= ~HP_SCNAME;
			EXCEPTIONAL_UNLOCK(PlayerHistory::mtx)
		}

#if !ASSUME_NO_RID_SPOOFING
		if (!is_rid_valid(historic_player->rid)
			&& historic_player_rid == nullptr
			)
		{
			historic_player->rid = rid;
		}
#endif

		if (auto mac_id = p.getMacId();
			mac_id != 0
			)
		{
			historic_player->mac_id = mac_id;
		}
		historic_player->setLastSeenNow();
	}

	void CommandPlayer::updateCachedVars()
	{
		if (!isSelf())
		{
			is_friend = pointers::g_rlPc->player_manager->IsFriend(rid);
			if (auto cngp = getPlayer().getCNetGamePlayer())
			{
				if (auto my_cngp = g_player.getCNetGamePlayer())
				{
					const auto clan_id = cngp->clan_membership.clan.id;
					is_crew_member = (clan_id != 0 && my_cngp->clan_membership.clan.id == cngp->clan_membership.clan.id);
				}
			}
		}
	}

	static const Vector3 cayo_perico_position = Vector3(4916.242f, -4917.842f, 46.777893f);
	static const Vector3 airport_position = Vector3(-1336.6625f, -3043.5588f, 13.944448f); // In the middle, where taxiways intersect

	void CommandPlayer::doWork(ThreadContext thread_context)
	{
		AbstractPlayer p(((const PlayerProviderSingle*)this->pp)->p);
		updateCachedVars();
		AbstractEntity ped = p.getPed();
		// Flags
		playerflag_t flags = 0;
		if (is_friend)
		{
			flags |= 1 << FLAG_FRIEND;
		}
		if (is_crew_member)
		{
			flags |= 1 << FLAG_CREW;
		}
		if (g_gui.isRootStateFull())
		{
			if (p.isHost())
			{
				flags |= 1 << FLAG_HOST;

#ifdef STAND_DEBUG
				// Incongruence 1
				if (((*pointers::network_session)->session->rline_session.data.info.token >> 32) != p.getMacId()
					&& (*pointers::network_session)->session->rline_session.data.info.token != 0 // happens when leaving session
					)
				{
					getAndApplyReactions(p, FlowEvent::MOD_INCONGRUENCE_1);
				}
#endif
			}
			else if (p.isNextHost())
			{
				flags |= 1 << FLAG_NEXT_HOST;
			}
			if (p.isScriptHost())
			{
				flags |= 1 << FLAG_SCRIPT_HOST;
			}

			SOUP_IF_LIKELY (g_gui.inited_netIntel)
			{
				flags |= 1 << FLAG_GEOIP;
			}

#ifdef STAND_DEBUG
			// Incongruence 2
			if (auto endpoint = p.getEndpoint())
			{
				if (endpoint->GetPeerId() != p.getGamerInfo()->peer.address.startup_id)
				{
					getAndApplyReactions(p, FlowEvent::MOD_INCONGRUENCE_2);
				}
			}
#endif
		}

		if (!is_session_transition_active(true, false))
		{
			if (ped.isValid())
			{
				auto* netobj = reinterpret_cast<CNetObjPlayer*>(ped.getPointer()->m_net_object);
				if (netobj != nullptr)
				{
					bool is_legit_spectating = false;
					if (netobj->m_SpectatorObjectId != 0)
					{
						if (auto n = (*pointers::network_object_mgr)->find_object_by_id(netobj->m_SpectatorObjectId, false))
						{
							// assuming target is a player ped, but since the way to get player from player ped is checking netObject owner, this should work fine in all cases
							if (n->owner_id != p) // can't spectate self
							{
								spectating = n->owner_id;

								is_legit_spectating = true; // skip ninja spectate detection
							}
						}
					}
					if (!is_legit_spectating)
					{
						const auto cam_pos = p.getCamPos();
						if (!p.isThisPositionInCameraOrbit(cam_pos))
						{
							if (!isSelf())
							{
								// Cherax puts the camera here when spectating via extended population range:
								// - 329, 14751, 23
								// - 362, 14789, 28
								// - 462, 14872, 44
								// - 466, 14863, 41
								if (cam_pos.y > 14'000.0f)
								{
									getAndApplyReactions(p, FlowEvent::MOD_MISC, Codename("M1").toString());
								}
							}

							if (spectating != MAX_PLAYERS)
							{
								if (!AbstractPlayer(spectating).isThisPositionInCameraOrbit(cam_pos))
								{
									spectating = MAX_PLAYERS;
								}
							}

							if (spectating == MAX_PLAYERS)
							{
								// Discover ninja spectate target based on distance & angle
								for (const auto& other : AbstractPlayer::listAll())
								{
									if (other != p) // can't spectate self
									{
										if (other.isThisPositionInCameraOrbit(cam_pos))
										{
											if (Util::angles_dist(
												p.getCamRot().z,
												cam_pos.lookAt(other.getPos()).z
											) <= 5.0f) // <= 5.0f when stationary, can vary up to 50.0f while target is turning
											{
												// Not same vehicle?
												auto veh = p.getPed().getVehicle();
												if (!veh.isValid()
													|| other.getPed().getVehicle() != veh
													)
												{
													// Peds are not close to each other? (To catch when passengers when the vehicle is not synched with us.)
													if (p.getPos().distance(other.getPos()) > 10.0f)
													{
														spectating = other;
														break;
													}
												}
											}
										}
									}
								}
							}
						}
						else
						{
							spectating = MAX_PLAYERS;
						}
					}
					if (spectating != MAX_PLAYERS)
					{
						flags |= 1 << FLAG_SPEC;
					}
				}
			}
			int health = 0;
			if (ped.isValid())
			{
				health = (int)ped.getHealth();
				if (health == 0)
				{
					flags |= 1 << FLAG_DEAD;
					if (!((this->flags >> FLAG_DEAD) & 1))
					{
						latest_death_at = get_current_time_millis();
					}
					else if (GET_MILLIS_SINCE(latest_death_at) > 40000 && !((flags >> FLAG_SPEC) & 1))
					{
						getAndApplyReactions(p, FlowEvent::MOD_LONG_DEATH);
					}
				}

				if (ped.isInvulnerable())
				{
					flags |= 1 << FLAG_GOD;
					if (invulnerable_since == 0)
					{
						invulnerable_since = get_current_time_millis();
					}
				}
				else
				{
					invulnerable_since = 0;
				}
			}
			if (p.isOutOfSight())
			{
				flags |= 1 << FLAG_OTR;
				const auto pos = ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_IMANI_OUTOFSIGHT_POS).get<Vector3>();
				if (!last_outofsight_pos.isNull()
					&& last_outofsight_pos != pos
					)
				{
					getAndApplyReactions(p, FlowEvent::MOD_LONG_OTR, 25);
				}
				last_outofsight_pos = pos;
			}
			else
			{
				if (!last_outofsight_pos.isNull())
				{
					last_outofsight_pos.reset();
				}
				if (p.isOtr())
				{
					flags |= 1 << FLAG_OTR;
					if (!p.getCurrentVehicleModel().isStealthVehicle())
					{
						if (!otr_deadline)
						{
							otr_deadline = get_current_time_millis() + (p.isInAnOrg() ? 240000 : 60000) + 3000;
						}
						else if (IS_DEADLINE_REACHED(otr_deadline))
						{
							getAndApplyReactions(p, FlowEvent::MOD_LONG_OTR, 25);
						}
					}
					else
					{
						otr_deadline = 0;
					}
				}
				else
				{
					otr_deadline = 0;
				}
			}
			if (p.hasBounty())
			{
				last_had_bounty_at = get_current_time_millis();
				last_bounty_value = p.getBountyValue();
			}
			if (p.isInInterior())
			{
				flags |= 1 << FLAG_INT;
			}
			if (p.isTyping())
			{
				flags |= 1 << FLAG_TYPING;
			}
			if (auto veh = p.getVehicle(); veh.isValid())
			{
				if (veh.isInvulnerable())
				{
					flags |= 1 << FLAG_VEHGOD;
				}
			}
			if (p.isInVehicle())
			{
				flags |= 1 << FLAG_INVEH;
				if (!latest_vehicle_entry_at)
				{
					latest_vehicle_entry_at = get_current_time_millis();
				}
			}
			else
			{
				latest_vehicle_entry_at = 0;
			}
			if (p.isUsingRcVehicle())
			{
				flags |= 1 << FLAG_RC;
			}

			if (!isSelf())
			{
				// Desync Kick Karma
				if (isTimingOut())
				{
					LeaveReasons::getEntry(p.getPeerId()).was_timing_out = true;
					if (desync_kick_karma
						&& joined
						&& !g_player.isHost()
						&& !p.isHost()
						)
					{
						p.kickLoveLetter(false);
					}
				}

				if (joined)
				{
					if (!is_session_transition_active(true, false))
					{
						// Stand User Identification
						if (!attempted_stand_user_handshake
							&& g_auth.remote_session == SessionSpoofing::getRealSessionId()
							)
						{
							uint8_t auth_info = p.getAuthInfo();
							if (auth_info != 0
								&& (auth_info == 2 || g_auth.isSuiEnabledThisSession())
								)
							{
#if DEBUG_SUI_HANDSHAKE
								Util::toast(fmt::format("Attempting SUI handshake with {}", p.getName()));
#endif
								ColoadMgr::check();
								ExecCtx::get().ensureScript([p]
								{
									p.directPacketSendIdentify(DP_HANDSHAKE);
								});
								sent_stand_user_handshake_at = get_current_time_millis();
							}
							attempted_stand_user_handshake = true;
						}
					}

					if (is_session_started_and_transition_finished())
					{
						// Leave Reasons
						if (ScriptGlobal(GLOBAL_VOTE_KICKED).at(p, 1).get<int>() == 1
							&& p.canBeVoteKickedNaive()
							&& p.hasEnoughVotesToKick()
							)
						{
							LeaveReasons::getEntry(p.getPeerId()).vote_kicked = true;
						}

						// RID Spoofing Lookup
						if (!did_rid_spoof_lookup)
						{
							auto* g = RemoteGamer::get(rid, true, RGU_NONE);
							if (g == nullptr)
							{
								concludeRidChecks(0);
							}
							else
							{
								const bool check_data = (!g->isQueuedOrPending() && g->hasResponse()
									&& g->response_from_detailed
								);
								const bool times_up = (GET_MILLIS_SINCE(discovery) > 50000);
								if (!times_up)
								{
									g->considerRefresh(true);
								}
								if (check_data)
								{
									if (g->session_info.id == SessionSpoofing::getRealSessionId())
									{
										removeDetection(FlowEvent::MOD_PRESENCESPOOFING);
										concludeRidChecks(rid);
										if (!g->hasPeerAddr())
										{
											getAndApplyReactions(p, FlowEvent::MOD_PRESENCESPOOFING);
										}
									}
									else if (times_up)
									{
										getAndApplyReactions(p, FlowEvent::MOD_PRESENCESPOOFING, 75);
										concludeRidChecks(0);
#if ASSUME_NO_RID_SPOOFING
										session_spoofing = true;
#endif
									}
								}
							}
						}
						else
						{
#if ASSUME_NO_RID_SPOOFING
							if (!session_spoofing)
							{
								trampoline_rid = rid;
							}
#else
							if (real_rid != 0)
							{
								trampoline_rid = real_rid;
							}
#endif
						}

						if (p.inOurOrg())
						{
							flags |= 1 << FLAG_ORG_MEMBER;
						}
					}

					// Interior Spoofing (X-Force does this; something related to their user identification?)
					if (p.getShopId() == 0
						&& p.getPos().distanceTopdown<rage::Vector2>({ -817.5519f, -185.32222f }) > 20.0f
						)
					{
						getAndApplyReactions(p, FlowEvent::MOD_MISC, Codename("M3").toString());
					}
				}
			}

			// Damage Multiplier
			if (is_session_started()
				&& is_session_freeroam()
				&& (!isSelf() || !g_hooking.block_outgoing_damage_mod)
				&& (!Util::isWeaponDamageModifierLegitimate(p.getWeaponDamageModifier()) || !Util::isMeleeDamageModifierLegitimate(p.getMeleeWeaponDamageModifier()))
				)
			{
				getAndApplyReactions(p, FlowEvent::MOD_DMGMUL);
			}

			if (is_session_started_and_transition_finished())
			{
				// Hidden From Player List
				if (p.inCayo())
				{
					// https://www.guilded.gg/stand/groups/x3ZgB10D/channels/47eb8ea9-21be-4642-8916-b062b4f9cac0/forums/1538681970
					// - Cayo loading screen for Velum entrance. (4000.0f = beach south of oil fields)
					// - Cayo loading screens at the airport. (covers the entire airport. so, initial hanger loading screen, flight school entrance, apparently there's another nimbus loading screen too)
					constexpr float required_distance_from_cayo = 4000.0f;
					constexpr float required_distance_from_airport = 600.0f;

					if (auto blip = p.getBlipPos(); !blip.isNull())
					{
						const auto ped_pos = ped.getPos();

						if (blip.distance(airport_position) > required_distance_from_airport
							&& blip.distance(cayo_perico_position) > required_distance_from_cayo
							&& (!ped.isValid() || ped_pos.distance(airport_position) > required_distance_from_airport)
							&& (!ped.isValid() || ped_pos.distance(cayo_perico_position) > required_distance_from_cayo)
							)
						{
							if (cayo_check_timer == 0)
							{
								cayo_check_timer = get_current_time_millis();
							}
							else if (GET_MILLIS_SINCE(cayo_check_timer) > 30000)
							{
								p.triggerDetection(FlowEvent::MOD_CAYO);
								cayo_check_timer = 0;
							}
						}
					}
				}
				else
				{
					cayo_check_timer = 0;
				}

				if (joined
					&& p.areStatsReady()
					&& !p.areStatsBusted()
					)
				{
					// Spoofed RP
					int rank = p.getRank();
					int rp = p.getRp();
					if (rank < 0 || rank > 8000 || rp < 0 || rp > 1787576850)
					{
						// Invalid Rank/RP
						getAndApplyReactions(p, FlowEvent::MOD_RP2);
					}
					if (rp < Util::get_rp_required_for_rank(rank)
						|| (
							rp > Util::get_rp_required_for_rank(rank + 1)
							&& rank != 8000
							)
						)
					{
						// RP-Rank Mismatch
						getAndApplyReactions(p, FlowEvent::MOD_RP3);
					}
					if (!isSelf()
						&& ped.isValid()
						)
					{
						auto netobj = (CNetObjPlayer*)ped.getCPed()->m_net_object;
						if (netobj)
						{
							const auto cnop_rank = netobj->m_stats[PlayerCard::NETSTAT_RANK];
							if (cnop_rank != 0)
							{
								if (cnop_rank != rank)
								{
									if (cnop_rank > 0.0000000001f || cnop_rank < 0)
									{
										if (rank_incongruence_since == 0)
										{
											rank_incongruence_since = get_current_time_millis();
										}
										else if (GET_MILLIS_SINCE(rank_incongruence_since) > 20000)
										{
											// Spoofed Rank
											getAndApplyReactions(p, FlowEvent::MOD_RP4, 50); // lower severity because this updates somewhat unreliably
										}
									}
								}
								else
								{
									rank_incongruence_since = 0;
								}
							}
						}
					}
					// Modded Organisation Name
					if (p.isBoss()
						&& strpbrk(p.getOrgName(), "<>~") != nullptr
						)
					{
						// If the name of the org was changed *right now*, then we could say it's severity 100.
						auto reactions = p.getReactions(FlowEvent::MOD_ORGNAME);
						applyReactions(reactions, NuancedEvent(FlowEvent::MOD_ORGNAME, 75));
						if (reactions & REACTION_BLOCK)
						{
							std::string str = p.getOrgName();
							StringUtils::replace_all(str, "<", "");
							StringUtils::replace_all(str, ">", "");
							StringUtils::replace_all(str, "~", "");
							const bool is_mc = p.isInMc();
							beginBadSgTamperCodeThatWillLandUsInHell();
							p.setOrgName(is_mc, str.c_str());
							endBadSgTamperCodeThatWillLandUsInHell();
						}
					}
				}
			}

			if (ped.isValid())
			{
				const AbstractModel model = syncing_as_model;
				if (model.isPlayableOnlineCharacter())
				{
					// Modded Health
					if (is_session_freeroam() && p.areStatsReady())
					{
						const auto max_health = (int)ped.getMaxHealth();
						if (GET_MILLIS_SINCE(discovery) > 70'000)
						{
							if (health < 0
								|| health > max_health
								|| !Util::isMaxHealthLegitimate(max_health)
								)
							{
								getAndApplyReactions(p, FlowEvent::MOD_HEALTH, fmt::format("{}/{}", health, max_health));
							}
						}
					}
				}
				else
				{
					// Modded Character Model
					if (!(
							(/*ManagedTunables::peyotes.getRealValue() &&*/ model.isObtainableFromPeyote())
							&& (ped.isInWater() == model.isWaterAnimal())
						)
						&& is_session_started_and_transition_finished()
						&& (!is_session_freeroam() || model != ATSTRINGHASH("a_c_rabbit_02")) // First Dose 4 - Uncontrolled Substance
						)
					{
						getAndApplyReactions(p, FlowEvent::MOD_MODEL, joaatToString(model));
					}
				}

				// Too much armour
				{
					float armor = ped.getArmour();
					if (armor < 0.0f || armor >(is_session_started_and_transition_finished() ? 50.0f : 100.0f))
					{
						getAndApplyReactions(p, FlowEvent::MOD_ARMOUR);
					}
				}
			}
		}

		if (isSelf())
		{
			if (pointers::ros_credentials->HasPrivilege(rage::rlRosPrivilegeId::DEVELOPER))
			{
				getAndApplyReactions(p, FlowEvent::MOD_RSDEV);
			}
			if (g_hooking.spoof_qa_flag)
			{
				getAndApplyReactions(p, FlowEvent::MOD_RSQA);
			}
			if (g_hooking.spoof_cheater_flag)
			{
				getAndApplyReactions(p, FlowEvent::MOD_CHEAT);
			}

			if (!ENTITY::IS_ENTITY_VISIBLE(g_player_ped))
			{
				flags |= (1 << FLAG_INVISIBLE);
			}
		}
		else
		{
			if (hasStandUserHandshakeFinished())
			{
				if (const CNetGamePlayer* const cnetgameplayer = p.getCNetGamePlayer())
				{
					if (auto cheats_notified = (cnetgameplayer->m_cheatsNotified & ~(1 << 3)))
					{
						getAndApplyReactions(p, FlowEvent::MOD_RAC, fmt::to_string(soup::bitutil::getLeastSignificantSetBit(cheats_notified)), 50);
					}
					if (cnetgameplayer->IsRockstarDev())
					{
						getAndApplyReactions(p, FlowEvent::MOD_RSDEV);
					}
					if (cnetgameplayer->IsRockstarQA())
					{
						getAndApplyReactions(p, FlowEvent::MOD_RSQA);
					}
					if (cnetgameplayer->IsCheater())
					{
						getAndApplyReactions(p, FlowEvent::MOD_CHEAT);
					}
				}
			}

			if (!syncing_as_visible)
			{
				flags |= (1 << FLAG_INVISIBLE);
			}
		}

		// Chatting Without Typing Indicator
		if (p.isTyping())
		{
			last_typing_at = get_current_time_millis();
		}

		// Super Jump
		if (const CPlayerInfo* const player_info = p.getPlayerInfo())
		{
			if (player_info->m_frame_flags & (
				(1 << 11) // explosive ammo
				| (1 << 12) // fire ammo
				| (1 << 13) // explosive melee
				))
			{
				getAndApplyReactions(p, FlowEvent::MOD_FROG, "*");
			}

			if (((player_info->m_frame_flags >> 14) & 1)
#if HAVE_SUPERJUMP_ANTI_DETECTION
				&& (!isSelf() || !g_hooking.block_outgoing_super_jump_flag)
#endif
				&& (!is_session_started()
					|| is_session_freeroam()
					)
				)
			{
				getAndApplyReactions(p, FlowEvent::MOD_FROG);
			}
		}
		if (ped.isValid())
		{
			if (auto intelligence = ped.getCPedRaw()->intelligence)
			{
				if (auto pTaskJump = static_cast<CTaskJump*>(intelligence->FindTaskActiveByType(CTaskTypes::TASK_JUMP)))
				{
					if ((pTaskJump->flags & JF_SuperJump) || (pTaskJump->flags & JF_BeastJump))
					{
						if (!p.isBeast())
						{
							getAndApplyReactions(p, FlowEvent::MOD_FROG);
						}
					}
				}
			}
		}

		// Timeout
		if (p.isInTimeout())
		{
			if (g_hooking.player_timeouts[p] < get_current_time_millis())
			{
				setTimeoutState(thread_context, false);
			}
			else
			{
				flags |= (1 << FLAG_TIMEOUT);
			}
		}

		// Copy over flags that will only be applied once
		flags |= (this->flags & (
			(1 << FLAG_SELF)
			| (1 << FLAG_LIKELY_MODDER)
			| (1 << FLAG_MOD)
#if HAS_MODAMN
			| (1 << FLAG_MODAMN)
#endif
			| (1 << FLAG_AMN)
			| (1 << FLAG_ATTACKED)
			| (1 << FLAG_VPN)
			| (1 << FLAG_STAND_USER)
			));

		// Check if update is needed
		if (force_flag_update || flags != this->flags)
		{
			this->flags = flags;
			updateFlags(p);
		}
		else if (help_text_connected_via != p.getConnectedViaForPlayerHelpText()
			|| help_text_note != getNote()
			)
		{
			setHelpText(LIT(getHelpText(p)));
		}
	}

	CommandPlayerClassifier* CommandPlayer::getDetectionsList() const noexcept
	{
		return getChild(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandPlayerClassifier>();
	}

	void CommandPlayer::updateClassification(CommandPlayerClassifier* const detections)
	{
		auto classification = classifier.classify();
		std::string menu_name = LANG_GET("CLSFN");
		menu_name.append(": ");
		menu_name.append(PlayerClassifier::getLocalisedClassificationName(classification));
		detections->setMenuName(LIT(std::move(menu_name)));
		switch (classification)
		{
		default:
			detections->setHelpText(NOLABEL);
			break;

		case PlayerClassifier::NONE:
			detections->setHelpText(LOC("CLSFN_N_H"));
			break;

#if HAS_MODAMN
		case PlayerClassifier::MODDER_OR_ADMIN:
			detections->setHelpText(LOC("CLSFN_MA_H"));
			break;
#endif
		}
		const bool can_show_tag = (classification_flags_on_self || !isSelf());
		setFlag(FLAG_LIKELY_MODDER, can_show_tag && classification == PlayerClassifier::LIKELY_MODDER);
		setFlag(FLAG_MOD, can_show_tag && classification == PlayerClassifier::MODDER);
#if HAS_MODAMN
		setFlag(FLAG_MODAMN, can_show_tag && classification == PlayerClassifier::MODDER_OR_ADMIN);
#endif
		setFlag(FLAG_AMN, can_show_tag && classification == PlayerClassifier::ADMIN);
	}

	void CommandPlayer::updateClassification()
	{
		return updateClassification(getDetectionsList());
	}

	floweventreaction_t CommandPlayer::checkClassificationUpdate(PlayerClassifier::Classification& classification, const PlayerClassifier::Classification prev_classification) const
	{
		classification = classifier.classify();

		if (classification != prev_classification
			&& classification != PlayerClassifier::NONE
			)
		{
			const auto p = getPlayer();

			auto classification_reactions = p.getReactions(FlowEvent::CLSFN_ANY);
			classification_reactions |= p.getReactions(FlowEvent::CLSFN_ANY + classification);
			return classification_reactions;
		}

		return 0;
	}

	void CommandPlayer::notifyClassificationUpdate(PlayerClassifier::Classification classification, toast_t toast_flags, floweventreaction_t reactions) const
	{
		if (toast_flags != 0)
		{
			std::string toast = LANG_FMT("CLSFN_T", FMT_ARG("player", getPlayerName()), FMT_ARG("classification", PlayerClassifier::getLocalisedClassificationName(classification)));
			AbstractPlayer::augmentReactionToast(toast, reactions);
			Util::toast(std::move(toast), toast_flags);
		}
	}

	bool CommandPlayer::getFlag(playerflag_t id) const noexcept
	{
		//return flags & (1 << id);
		return (flags >> id) & 1;
	}

	void CommandPlayer::setFlag(playerflag_t id, bool val) noexcept
	{
		if (getFlag(id) != val)
		{
			if (val)
			{
				flags |= (1 << id);
			}
			else
			{
				flags &= ~(1 << id);
			}
			updateFlags();
		}
	}

	void CommandPlayer::updateFlags()
	{
		return updateFlags(getPlayer());
	}

#define APPEND_FLAG(id, name) \
if (flags & (1 << id)) \
{ \
	const char32_t flag_char = flag_chars[id]; \
	if (flag_char != 0) \
	{ \
		menu_name_add.append(soup::unicode::utf32_to_utf8(flag_char)); \
		StringUtils::list_append(help_text_add, LANG_GET(name)); \
	} \
}

	void CommandPlayer::updateFlags(AbstractPlayer p)
	{
		if (blip_names)
		{
			p.updateNameOnBlip();
		}

		menu_name.setLiteral(p.getName());
		help_text.setLiteral(getHelpText(p));
		std::string menu_name_add, help_text_add;
		FOR_ALL_FLAGS(APPEND_FLAG);
		if (!menu_name_add.empty())
		{
			menu_name.literal_str.append(StringUtils::utf16_to_utf8(flag_prefix)).append(menu_name_add).append(StringUtils::utf16_to_utf8(flag_suffix));
			if (!help_text.literal_str.empty())
			{
				help_text.literal_str.push_back('\n');
			}
			help_text.literal_str.append(LANG_GET("FLAGS")).append(": ").append(help_text_add);
		}
		processVisualUpdate();
		PlayerListSort::flags_have_changed_since_last_sort = true;
	}

	std::string CommandPlayer::getHelpText(AbstractPlayer p)
	{
		std::string help_text{};
		if (g_gui.isRootStateFull())
		{
			help_text = soup::ObfusString("Rockstar ID: ").str();
			help_text.append(p.getRockstarIdForUser<std::string>());
			help_text_connected_via = p.getConnectedViaForPlayerHelpText();
			if (!help_text_connected_via.empty())
			{
				help_text.push_back('\n');
				help_text.append(help_text_connected_via);
			}
			if (p.getAddress().ip != -1)
			{
				help_text.push_back('\n');
				help_text.append(LANG_GET("IP")).append(": ").append(p.getIpAddressForUserA());
				help_text.append(soup::ObfusString("\nGeoIP: ").str());
				if (p.isInScope(AbstractPlayer::hide_geoip))
				{
					help_text.append(LANG_GET("RDCTD"));
				}
				else if (!g_gui.inited_netIntel)
				{
					help_text.append(LANG_GET("GENWAIT"));
				}
				else if (auto location = g_gui.getNetIntel().getLocationByIpv4(p.getAddress().ip.value))
				{
					if (*location->city)
					{
						help_text.append(location->city);
						help_text.append(", ");
					}
					if (*location->state)
					{
						help_text.append(location->state);
						help_text.append(", ");
					}
					if (auto cn = soup::getCountryName(location->country_code.data(), Lang::activeToCodeUpperForGeoip()))
					{
						help_text.append(cn);
					}
					else
					{
						help_text.append(location->country_code.data());
					}
				}
				else
				{
					help_text.append("???");
				}
				help_text.append(soup::ObfusString("\nISP: ").str());
				if (p.isInScope(AbstractPlayer::hide_geoip))
				{
					help_text.append(LANG_GET("RDCTD"));
				}
				else if (!g_gui.inited_netIntel)
				{
					help_text.append(LANG_GET("GENWAIT"));
				}
				else if (auto as = g_gui.getNetIntel().getAsByIpv4(p.getAddress().ip.value))
				{
					help_text.append(as->name);
					if (as->isHosting(g_gui.getNetIntel()))
					{
						flags |= (1 << FLAG_VPN);
					}
				}
				else
				{
					help_text.append("???");
				}
				help_text_note = getNote();
				if (!help_text_note.empty())
				{
					std::string note = help_text_note;
					soup::string::truncateWithEllipsis(note, 75);
					help_text.push_back('\n');
					help_text.append(LANG_GET("PLYNOTE") + ": " + note);
				}
			}
		}
		return help_text;
	}

	std::string CommandPlayer::getNote() const
	{
		if (historic_player)
		{
			return historic_player->note;
		}
		return {};
	}

	floweventreaction_t CommandPlayer::getReactions(flowevent_t event) const
	{
		return getPlayer().getReactions(event);
	}

	void CommandPlayer::applyReactions(floweventreaction_t reactions, const NuancedEvent& event, bool batched)
	{
		applyReactions(reactions, event, FlowEvent::getName(event.type), {}, batched);
	}

	void CommandPlayer::applyReactions(floweventreaction_t reactions, const NuancedEventWithExtraData& event, bool batched)
	{
		applyReactions(reactions, event, FlowEvent::getName(event.type), event.extra_data, batched);
	}

	void CommandPlayer::applyReactions(floweventreaction_t reactions, const NuancedEvent& event, Label event_name, const std::string& extra_data, bool batched)
	{
		evtFlowEventDoneEvent::trigger(evtFlowEventDoneEvent(getPlayer(), event.type, event_name, extra_data));

		const Label name = FlowEvent::getName(std::move(event_name), extra_data);

		PlayerClassifier::Classification prev_classification;
		PlayerClassifier::Classification classification;
		if (!batched)
		{
			prev_classification = classifier.classify();
		}

		toast_t event_tf = 0;
		toast_t classification_tf = 0;
		floweventreaction_t classification_reactions = 0;

		if (addOrUpdateDetection(event, name))
		{
			event_tf = flow_event_reactions_to_toast_flags(reactions);
		}

		if (!batched)
		{
			classification_reactions = checkClassificationUpdate(classification, prev_classification);
			if (classification_reactions)
			{
				if (event.type == FlowEvent::MOD_ID_STAND)
				{
					classification_reactions &= ~REACTIONS_AGGRESSIVE;
				}
				classification_tf = flow_event_reactions_to_toast_flags(classification_reactions);
				reactions |= classification_reactions;
			}
		}

		toast_t shared_tf = (event_tf & classification_tf);
		event_tf &= ~shared_tf;
		classification_tf &= ~shared_tf;

		if (shared_tf != 0)
		{
			std::string toast = LANG_FMT("DT_CLSFN", FMT_ARG("player", getPlayerName()), FMT_ARG("reason", name.getLocalisedUtf8()), FMT_ARG("classification", PlayerClassifier::getLocalisedClassificationName(classification)));
			AbstractPlayer::augmentReactionToast(toast, reactions);
			Util::toast(std::move(toast), shared_tf);
		}
		if (event_tf != 0)
		{
			std::string toast = LANG_FMT("DT_T", FMT_ARG("player", getPlayerName()), FMT_ARG("reason", name.getLocalisedUtf8()));
			AbstractPlayer::augmentReactionToast(toast, reactions);
			Util::toast(std::move(toast), event_tf);
		}
		notifyClassificationUpdate(classification, classification_tf, classification_reactions);

		applyReactions(reactions);
	}

	void CommandPlayer::applyReactions(floweventreaction_t reactions)
	{
		constexpr ThreadContext thread_context = TC_OTHER;

		if (
			(reactions & REACTION_BLACKLIST_KICK)
			&& g_player.isHost()
			)
		{
			FiberPool::queueJob([this]
			{
				auto p = getPlayer();
				rage::rlGamerHandle gh = p.getGamerHandle();
				p.kickHost(AbstractPlayer::DESYNC);
				(*pointers::network_session)->blacklist.AddGamer(gh);
			});
			return;
		}

		if (reactions & REACTION_KICK)
		{
			if (!did_kick_reaction)
			{
				did_kick_reaction = true;
				FiberPool::queueJob([this]
				{
#if ASSUME_NO_RID_SPOOFING
					// No need for handshake to complete because Stand admins are detected by RID.
#else
					while (!hasStandUserHandshakeFinished())
					{
						Script::current()->yield();
						if (queued_for_deletion)
						{
							return;
						}
					}
#endif
					getPlayer().kick(true, false);
				});
				return;
			}
		}
		
		if (reactions & REACTION_LL_KICK)
		{
			did_kick_reaction = true;
			FiberPool::queueJob([this]
			{
				auto p = getPlayer();
				if (!p.isHost())
				{
					if (g_player.isHost())
					{
						if (p.canUseUnblockableKick())
						{
							p.kickHost(AbstractPlayer::DESYNC);
						}
					}
					else
					{
						LeaveReasons::getEntry(p.getPeerId()).love_letter_from = g_player;
						p.kickLoveLetter(true);
					}
				}
			});
		}

		if (reactions & REACTION_CRASH)
		{
			if (!did_crash_reaction)
			{
				did_crash_reaction = true;
				FiberPool::queueJob([this]
				{
					getPlayer().crash();
				});
			}
			return;
		}

		if (reactions & REACTION_TIMEOUT)
		{
			putInTimeout(thread_context);
		}
		if (reactions & REACTION_NET_BLOCK)
		{
			blockIncomingNetEvents(thread_context);
		}
		if (reactions & REACTION_SYNCIN_BLOCK)
		{
			blockIncomingSyncs(thread_context);
		}
		if (reactions & REACTION_SYNCOUT_BLOCK)
		{
			blockOutgoingSyncs(thread_context);
		}
		if (reactions & REACTION_NET_LOG)
		{
			Click click(CLICK_AUTO, thread_context);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_NET")))->resolveChildByMenuName(LOC("RCT_L_F")))->setStateBool(click, true);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_NET")))->resolveChildByMenuName(LOC("RCT_L_C")))->setStateBool(click, true);
		}
		if (reactions & REACTION_SYNCIN_LOG)
		{
			Click click(CLICK_AUTO, thread_context);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_SI")))->resolveChildByMenuName(LOC("RCT_L_F")))->setStateBool(click, true);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_SI")))->resolveChildByMenuName(LOC("RCT_L_C")))->setStateBool(click, true);
		}
		if (reactions & REACTION_SYNCOUT_LOG)
		{
			Click click(CLICK_AUTO, thread_context);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_SO")))->resolveChildByMenuName(LOC("RCT_L_F")))->setStateBool(click, true);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_SO")))->resolveChildByMenuName(LOC("RCT_L_C")))->setStateBool(click, true);
		}
		if (reactions & REACTION_NET_TOAST)
		{
			Click click(CLICK_AUTO, thread_context);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_NET")))->resolveChildByMenuName(LOC("RCT_T")))->setStateBool(click, true);
		}
		if (reactions & REACTION_SYNCIN_TOAST)
		{
			Click click(CLICK_AUTO, thread_context);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_SI")))->resolveChildByMenuName(LOC("RCT_T")))->setStateBool(click, true);
		}
		if (reactions & REACTION_SYNCOUT_TOAST)
		{
			Click click(CLICK_AUTO, thread_context);
			((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_SO")))->resolveChildByMenuName(LOC("RCT_T")))->setStateBool(click, true);
		}
	}

	void CommandPlayer::getAndApplyReactions(AbstractPlayer ap, flowevent_t event, uint8_t severity)
	{
		applyReactions(ap.getReactions(event), NuancedEvent(event, severity));
	}

	void CommandPlayer::getAndApplyReactions(AbstractPlayer ap, flowevent_t event, std::string&& extra_data, uint8_t severity)
	{
		applyReactions(ap.getReactions(event), NuancedEventWithExtraData(event, std::move(extra_data), severity));
	}

	void CommandPlayer::getAndApplyReactions(AbstractPlayer ap, flowevent_t event, const Label& name)
	{
		applyReactions(ap.getReactions(event), NuancedEvent(event), name);
	}

	void CommandPlayer::putInTimeout(ThreadContext thread_context) const
	{
		if (!isSelf())
		{
			setTimeoutState(thread_context, true);
			g_hooking.putInTimeout(getPlayer());
		}
	}

	void CommandPlayer::setTimeoutState(ThreadContext thread_context, bool state) const
	{
		Click click(CLICK_AUTO, thread_context);
		resolveChildByMenuName(LOC("TIMEOUT"))->as<CommandToggleNoCorrelation>()->setStateBool(click, state);
	}

	void CommandPlayer::blockIncomingNetEvents(ThreadContext thread_context) const
	{
		Click click(CLICK_AUTO, thread_context);
		((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_NET")))->resolveChildByMenuName(LOC("PTX_NET_R_B")))->setStateBool(click, true);
	}

	void CommandPlayer::blockIncomingSyncs(ThreadContext thread_context, bool state) const
	{
		Click click(CLICK_AUTO, thread_context);
		((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_SI")))->resolveChildByMenuName(LOC("PTX_NET_R_B")))->setStateBool(click, state);
	}

	void CommandPlayer::blockOutgoingSyncs(ThreadContext thread_context) const
	{
		Click click(CLICK_AUTO, thread_context);
		((CommandToggleNoCorrelation*)((CommandList*)resolveChildByMenuName(LOC("PTX_SO")))->resolveChildByMenuName(LOC("PTX_NET_R_B")))->setStateBool(click, true);
	}

	bool CommandPlayer::addOrUpdateDetection(const NuancedEvent& event, Label name)
	{
		if (FlowEvent::is_classifier(event.type))
		{
			classifier.addEvent(event);

			auto* const detections = getDetectionsList();
			if (detections != nullptr)
			{
				updateClassification(detections);

				auto* cmd = detections->getDetection(event.type);
				if (cmd == nullptr)
				{
					cmd = detections->children.emplace(detections->children.begin(), detections->makeChild<CommandPlayerClassifierDetection>(event.type, std::move(name), event.severity))->get()->as<CommandPlayerClassifierDetection>();
					detections->processChildrenUpdate();
					return !cmd->preemptive;
				}
				else
				{
					if (cmd->preemptive)
					{
						cmd->updateSeverity(event.severity);
						return !cmd->preemptive;
					}
				}
			}
		}
		return false;
	}

	void CommandPlayer::removeDetection(const flowevent_t type)
	{
		const auto prev_classification = classifier.classify();
		classifier.removeEvent(type);
		if (auto dtlst = getDetectionsList())
		{
			if (auto dtcmd = dtlst->getDetection(type))
			{
				dtcmd->onDetectionRemoved(TC_OTHER);
			}
			PlayerClassifier::Classification classification;
			if (auto reactions = checkClassificationUpdate(classification, prev_classification))
			{
				updateClassification(dtlst);
				notifyClassificationUpdate(classification, flow_event_reactions_to_toast_flags(reactions), reactions);
				applyReactions(reactions);
			}
		}
	}

	void CommandPlayer::directPacketReceived(DirectPacket a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6)
	{
		//g_logger.log(fmt::format("direct packet: {}", a0));
		if (a0 == DP_DEV_CLEAR_DETECT)
		{
			if (isMarkedAsStandUser())
			{
				flags &= ~(1 << FLAG_STAND_USER);
				if (classifier.hasEvent(FlowEvent::MOD_ID_STAND))
				{
					removeDetection(FlowEvent::MOD_ID_STAND);
				}
				else //if (classifier.hasEvent(FlowEvent::MOD_ID_STAND_COLOADING))
				{
					removeDetection(FlowEvent::MOD_ID_STAND_COLOADING);
				}
			}
		}
		else if (!isMarkedAsStandUser()
#if !SUI_DETECT_WHEN_OFF
			&& (g_auth.isSuiEnabledThisSession() || a0 == DP_DEV_FORCE_DETECT)
#endif
			)
		{
			flags |= 1 << FLAG_STAND_USER;
#if !SUI_DETECT_WHEN_OFF
			if (a0 != DP_DEV_FORCE_DETECT)
			{
				g_auth.stand_user_identification_this_session = true;
			}
#endif
			flowevent_t event_id = FlowEvent::MOD_ID_STAND;
			std::string detection_name = LANG_GET("DT_SU");
			std::string detection_extra{};
#ifdef STAND_DEBUG
			if (!(dp_flags & DPFLAG_SUION))
			{
				detection_extra = "S.U.I. Off";
			}
#endif
			if (dp_flags & DPFLAG_COLOADING)
			{
				event_id = FlowEvent::MOD_ID_STAND_COLOADING;
				StringUtils::list_append(detection_extra, LANG_GET("CLNG"));
			}
#ifdef STAND_DEBUG
			if (dp_flags & DPFLAG_DEVDBG)
			{
				StringUtils::list_append(detection_extra, "devdbg");
			}
#endif
			if (dp_flags & DPFLAG_DETASDEV)
			{
				StringUtils::list_append(detection_extra, soup::ObfusString("Developer").str());
			}
			if (!detection_extra.empty())
			{
				detection_name.append(" (").append(std::move(detection_extra)).append(1, ')');
			}
			getAndApplyReactions(getPlayer(), event_id, LIT(std::move(detection_name)));
		}
		switch (a0)
		{
		case DP_HANDSHAKE:
#if DEBUG_SUI_HANDSHAKE
			Util::toast(fmt::format("SUI handshake from {}", getPlayerName()));
#endif
			if (g_auth.isSuiEnabledThisSession()
				|| (dp_flags & DPFLAG_DEVDBG)
				)
			{
				ExecCtx::get().ensureScript([this]
				{
					getPlayer().directPacketSendIdentify(DP_I_GOT_YOU);
#ifdef STAND_DEBUG
					if (dp_flags & DPFLAG_SUION)
					{
#if SUI_DO_RESET_DETECT
						getPlayer().directPacketSend(DP_DEV_CLEAR_DETECT);
						getPlayer().directPacketSend(DP_DEV_FORCE_DETECT);
#endif
					}
					else
					{
#if SUI_DO_FORCE_DETECT
						getPlayer().directPacketSend(DP_DEV_FORCE_DETECT);
#endif
					}
#endif
				});
			}
			break;

		case DP_I_GOT_YOU:
#if DEBUG_SUI_HANDSHAKE
			Util::toast(fmt::format("SUI confirmation from {}", getPlayerName()));
#endif
#ifdef STAND_DEBUG
			ExecCtx::get().ensureScript([this]
			{
				if (dp_flags & DPFLAG_SUION)
				{
#if SUI_DO_RESET_DETECT
					getPlayer().directPacketSend(DP_DEV_CLEAR_DETECT);
					getPlayer().directPacketSend(DP_DEV_FORCE_DETECT);
#endif
				}
				else
				{
#if SUI_DO_FORCE_DETECT
					getPlayer().directPacketSend(DP_DEV_FORCE_DETECT);
#endif
				}
			});
#endif
			break;

		case DP_PONG_INFO:
			minigameColour = DirectX::SimpleMath::Color(
				float(a1) / 255.0f,
				float(a2) / 255.0f,
				float(a3) / 255.0f,
				float(a4) / 255.0f
			);
			their_pong_wins = a5;
			break;

		case DP_PONG_INVITE:
			if (Pong::participant == this)
			{
				ExecCtx::get().ensureScript([this]
				{
					sendPongInfo();
					getPlayer().directPacketSend(DP_PONG_INVITE_ACCEPTED);
				});
				pong_command->setMenuName(LOC("CNLGME_PONG"));
				Pong::i_am_left = a1;
				Pong::state = Pong::START_INTERMISSION;
				Pong::intermission();
			}
			else
			{
				if (a1)
				{
					ExecCtx::get().ensureScript([this]
					{
						getPlayer().directPacketSend(DP_PONG_INVITE_EXPIRED);
					});
				}
				if (!got_pong_invite)
				{
					got_pong_invite = true;
					Util::toast(LIT(LANG_FMT("INVRECV_PONG_T", FMT_ARG("player", getPlayerName()))), TOAST_DEFAULT);
					pong_command->setMenuName(LOC("INVRECV_PONG"));
				}
			}
			break;

		case DP_PONG_INVITE_ACCEPTED:
			if (Pong::participant == this)
			{
				ExecCtx::get().ensureScript([this]
				{
					sendPongInfo();
				});
				pong_command->setMenuName(LOC("CNLGME_PONG"));
				Pong::state = Pong::START_INTERMISSION;
				Pong::intermission();
			}
			break;

		case DP_PONG_INVITE_EXPIRED:
			if (Pong::participant == this && !Pong::i_am_left)
			{
				Pong::i_am_left = true;
				std::swap(Pong::left_platform_y, Pong::right_platform_y);
			}
			break;

		case DP_PONG_I_LOSE:
			if (a1 > 0) // >= 0.51
			{
				my_pong_wins = a1;
			}
			else
			{
				++my_pong_wins;
			}
			Pong::onPartyLoses();
			break;

		case DP_PONG_POS:
			if (Pong::participant == this)
			{
				*Pong::participantPlatformY() = int16_t(a1);
				if (!Pong::amIResponsibleForTheBall())
				{
					Pong::ball_x = int16_t(a2);
					Pong::ball_y = int16_t(a3);
					Pong::remote_ball_vec_x = Pong::ball_vec_x = int16_t(a4);
					Pong::ball_vec_y = int16_t(a5);
				}
			}
			break;

		case DP_PONG_BYE:
			if (Pong::participant == this)
			{
				Pong::stopGame();
				Util::toast(LIT(LANG_FMT("PLYGMECNCLT", getPlayerName())), TOAST_DEFAULT);
				pong_command->setMenuName(LOC("INVSEND_PONG"));
			}
			break;

		case DP_ADDR_REQUEST:
			ExecCtx::get().ensureScript([this, p{ (compactplayer_t)a1 }]
			{
				rage::netSocketAddress addr{};
				if (auto ep = AbstractPlayer(p).getEndpoint())
				{
					addr = ep->GetAddress().sock_addr;
				}
				getPlayer().directPacketSend(DP_ADDR_RESPONSE, *reinterpret_cast<const uint64_t*>(&addr), p);
			});
			break;

#ifdef STAND_DEBUG
		case DP_ADDR_RESPONSE:
			{
				rage::netSocketAddress addr{};
				*reinterpret_cast<uint64_t*>(&addr) = a1;
				Util::toast(fmt::format("{} says IP is {}", getPlayerName(), addr.ip.toString<std::string>()));
			}
			break;
#endif

#ifdef STAND_DEBUG
		case DP_HOSTSHARE_ACK:
			Util::toast(fmt::format("{} carried out our HostShare request.", getPlayerName()));
			break;
#endif

		case DP_HOSTSHARE_KICK_DESYNC:
			{
				ExecCtx::get().ensureScript([this, target{ (AbstractPlayer)(player_t)a1 }]
				{
					if (g_player.isHost()
						&& target != g_player
						&& eligibleForHostShare()
						)
					{
#ifdef STAND_DEBUG
						Util::toast(fmt::format("{} requested kick of {} via HostShare - Accepted.", getPlayerName(), target.getName()));
#endif
						target.kickHostNoprekick(AbstractPlayer::DESYNC);
						getPlayer().directPacketSend(DP_HOSTSHARE_ACK);
					}
					else
					{
#ifdef STAND_DEBUG
						Util::toast(fmt::format("{} requested kick of {} via HostShare - Denied.", getPlayerName(), target.getName()));
#endif
					}
				});
			}
			break;
		}
	}

	void CommandPlayer::sendPongInfo() const
	{
		getPlayer().directPacketSend(DP_PONG_INFO, (uint64_t)(g_renderer.minigameColour.x * 255.0f), (uint64_t)(g_renderer.minigameColour.y * 255.0f), (uint64_t)(g_renderer.minigameColour.z * 255.0f), (uint64_t)(g_renderer.minigameColour.w * 255.0f), my_pong_wins);
	}

	bool CommandPlayer::eligibleForHostShare() const noexcept
	{
		return hostshare_enabled
			&& (hostshare_strangers
				|| getPlayer().satisfiesAnyOf(hostshare_friends, hostshare_crew)
				);
	}
}
