#include "AbstractPlayer.hpp"

#include <fmt/core.h>

#include <soup/LcgRngInterface.hpp>
#include <soup/rand.hpp>
#include <soup/unicode.hpp>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "Auth.hpp"
#include "BattlEyeServer.hpp"
#include "BgScript.hpp"
#include "Blacklist.hpp"
#include "BlipUtil.hpp"
#include "CNetGamePlayer.hpp"
#include "CNetworkPlayerMgr.hpp"
#include "CNetworkSession.hpp"
#include "CollectablesGiver.hpp"
#include "ColoadMgr.hpp"
#include "CommandPlayer.hpp"
#include "CommandToggleNoCorrelation.hpp"
#include "conf.hpp"
#include "CPlayerInfo.hpp"
#include "DirectPacket.hpp"
#include "eDamageFlags.hpp"
#include "eOutOfSightFlags.hpp"
#include "EventAggregation.hpp"
#include "EventLabel.hpp"
#include "evtHostChangeEvent.hpp"
#include "ExecCtx.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "FlowEvent.hpp"
#include "FlowEventReaction.hpp"
#include "fmt_arg.hpp"
#include "fwPool.hpp"
#include "get_current_time_millis.hpp"
#include "get_ground_z.hpp"
#include "grcViewport.hpp"
#include "gta_net_object_mgr.hpp" // CNetObjPlayer
#include "gta_ped.hpp"
#include "Gui.hpp"
#include "HeistIslandTravelBitset.hpp"
#include "HistoricPlayer.hpp"
#include "homoglyphs.hpp"
#include "Hooking.hpp"
#include "IceQueen.hpp"
#include "is_rid.hpp"
#include "is_session.hpp"
#include "LeaveReasons.hpp"
#include "MsgTextMessage.hpp"
#include "name_validation.hpp"
#include "Names.hpp"
#include "natives.hpp"
#include "netComplaintMsg.hpp"
#include "netConnection.hpp"
#include "netEndpoint.hpp"
#include "NetworkSessionMessages.hpp"
#include "NuancedEventWithExtraData.hpp"
#include "PlayerType.hpp"
#include "pointers.hpp"
#include "Renderer.hpp"
#include "Script.hpp"
#include "script_events.hpp"
#include "ScriptGlobal.hpp"
#include "script_handler.hpp"
#include "ScriptLocal.hpp"
#include "ScriptHostUtil.hpp"
#include "script_packets.hpp"
#include "script_thread.hpp"
#include "scriptHandlerMgr.hpp"
#include "snet_messages.hpp"
#include "ThreadContext.hpp"
#include "TpUtil.hpp"
#include "TransitionHelper.hpp"
#include "TransitionState.hpp"
#include "tse.hpp"
#include "using_model.hpp"
#include "Util.hpp"
#include "yeet_flow.hpp"

namespace Stand
{
	CNetworkPlayerMgr* AbstractPlayer::getNetworkPlayerMgrIfApplicable()
	{
		if (auto mgr = *pointers::network_player_mgr)
		{
			if (is_session_started())
			{
				return mgr;
			}
			if (*pointers::CLoadingScreens_ms_Context == 0)
			{
				auto ts = ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>();
				if (is_session_transition_active(ts))
				{
					switch (ts)
					{
					default:
						return mgr;

					case TRANSITION_STATE_RETURN_TO_SINGLEPLAYER:
					case TRANSITION_STATE_WAIT_FOR_SINGLEPLAYER_TO_START:
						break;
					}
				}
			}
		}
		return nullptr;
	}

	void AbstractPlayer::onInit()
	{
		if (auto mgr = getNetworkPlayerMgrIfApplicable(); mgr && mgr->localPlayer)
		{
			host = mgr->getHostIndex();
		}
	}

	void AbstractPlayer::doWork()
	{
		is_freemode = SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(ATSTRINGHASH("freemode")) != 0;
		if (auto mgr = getNetworkPlayerMgrIfApplicable(); mgr && mgr->localPlayer)
		{
			g_player.p = mgr->localPlayer->player_id;

			AbstractPlayer cur_script_host = is_freemode ? NETWORK::NETWORK_GET_HOST_OF_SCRIPT("freemode", -1, 0) : -1;
			if (script_host != cur_script_host)
			{
				if (cur_script_host.isValid()
					&& g_gui.players_discovered
					)
				{
					floweventreaction_t reactions = cur_script_host.getReactions(FlowEvent::MISC_SCRIPTHOSTCHANGE);
					Util::toast(LANG_FMT("SHR_T", cur_script_host.getName()), flow_event_reactions_to_toast_flags(reactions));
					cur_script_host.applyReactions(reactions);
				}
				script_host = cur_script_host;
			}

			{
				auto p = mgr->getHostIndex();

				if (host != p)
				{
					prev_host = host;
					host = p;
					last_host_change = get_current_time_millis();

					if (host != MAX_PLAYERS
						&& g_gui.players_discovered
						)
					{
						evtHostChangeEvent::trigger(evtHostChangeEvent(host));

						AbstractPlayer host_ap(host);

						floweventreaction_t reactions = host_ap.getReactions(FlowEvent::MISC_HOSTCHANGE);
						Util::toast(LANG_FMT("HOSTR_T", host_ap.getName()), flow_event_reactions_to_toast_flags(reactions));
						host_ap.applyReactions(reactions);

						if (g_player != host_ap)
						{
							if (g_hooking.block_outgoing_syncs_to_host)
							{
								host_ap.sendCloneDelete(g_player_ped.getNetObjId());
							}

							if (BattlEyeServer::isRunningDueToUs())
							{
								FiberPool::queueJob([]
								{
									if (BattlEyeServer::isRunningDueToUs())
									{
										BattlEyeServer::stop();
									}
								});
							}
						}
					}

					if (g_player == host)
					{
						if (!BattlEyeServer::isRunning())
						{
							BattlEyeServer::start();
						}
					}
				}
			}
		}
		else
		{
			g_player.p = 0;
			host = -1;
			script_host = -1;

			if (BattlEyeServer::isRunningDueToUs())
			{
				FiberPool::queueJob([]
				{
					if (BattlEyeServer::isRunningDueToUs())
					{
						BattlEyeServer::stop();
					}
				});
			}
		}
	}

	void AbstractPlayer::updateLocalPlayer()
	{
		if (auto mgr = getNetworkPlayerMgrIfApplicable(); mgr && mgr->localPlayer)
		{
			g_player.p = mgr->localPlayer->player_id;
		}
		else
		{
			g_player.p = 0;
		}
	}

	int AbstractPlayer::getPlayerCount()
	{
		return is_session_or_transition_active(false) ? (*pointers::network_player_mgr)->NumPlayers : 1;
	}

	std::vector<AbstractPlayer> AbstractPlayer::listAll()
	{
		if (auto mgr = getNetworkPlayerMgrIfApplicable())
		{
			std::vector<AbstractPlayer> players = {};
			for (compactplayer_t p = 0; p != MAX_PLAYERS; ++p)
			{
				if (mgr->Players[p] != nullptr)
				{
					players.emplace_back(p);
				}
			}
			return players;
		}
		return { AbstractPlayer(0) }; // PLAYER::PLAYER_ID() is always 0 in story mode
	}

	std::vector<AbstractPlayer> AbstractPlayer::listOnly(const bool include_self, const bool include_friends, const bool include_crew_members, const bool include_stand_users, const bool include_org_members, const bool include_modders, const bool include_likely_modders)
	{
		if (auto mgr = getNetworkPlayerMgrIfApplicable())
		{
			std::vector<AbstractPlayer> players = {};
			for (compactplayer_t p = 0; p != MAX_PLAYERS; ++p)
			{
				if (mgr->Players[p] == nullptr)
				{
					continue;
				}
				AbstractPlayer ap(p);
				if ((include_self && p == g_player)
					|| ap.satisfiesAnyOf(include_friends, include_crew_members, include_stand_users, include_org_members, include_modders, include_likely_modders)
					)
				{
					players.emplace_back(ap);
				}
			}
			return players;
		}
		if (include_self)
		{
			return { AbstractPlayer(0) }; // PLAYER::PLAYER_ID() is always 0 in story mode
		}
		return {};
	}

	std::vector<AbstractPlayer> AbstractPlayer::listExcept(const bool exclude_self, const bool exclude_friends, const bool exclude_crew_members, const bool exclude_stand_users, const bool exclude_org_members, const bool exclude_modders, const bool exclude_likely_modders)
	{
		if (auto mgr = getNetworkPlayerMgrIfApplicable())
		{
			std::vector<AbstractPlayer> players = {};
			for (compactplayer_t p = 0; p != MAX_PLAYERS; ++p)
			{
				if (mgr->Players[p] == nullptr)
				{
					continue;
				}
				AbstractPlayer ap(p);
				if ((exclude_self && p == g_player)
					|| ap.satisfiesAnyOf(exclude_friends, exclude_crew_members, exclude_stand_users, exclude_org_members, exclude_modders, exclude_likely_modders)
					)
				{
					continue;
				}
				players.emplace_back(ap);
			}
			return players;
		}
		if (exclude_self)
		{
			return {};
		}
		return { AbstractPlayer(0) }; // PLAYER::PLAYER_ID() is always 0 in story mode
	}

	AbstractPlayer AbstractPlayer::getRandom()
	{
		return soup::rand(AbstractPlayer::listAll());
	}

	AbstractPlayer AbstractPlayer::getHost()
	{
		return host;
	}

	AbstractPlayer AbstractPlayer::getScriptHost()
	{
		return script_host;
	}

	AbstractPlayer AbstractPlayer::fromRockstarId(int64_t rid)
	{
		auto list = AbstractPlayer::listAll();
		for (auto& p : list)
		{
			if (p.getRockstarId() == rid)
			{
				return p;
			}
		}
		return invalid();
	}

	AbstractPlayer AbstractPlayer::fromStartupId(const rage::netPeerId peerId)
	{
		auto list = AbstractPlayer::listAll();
		for (auto& p : list)
		{
			if (p.getStartupId() == peerId)
			{
				return p;
			}
		}
		return invalid();
	}

	AbstractPlayer AbstractPlayer::fromPeerId(const rage::netPeerId peerId)
	{
		auto list = AbstractPlayer::listAll();
		for (auto& p : list)
		{
			if (p.getPeerId() == peerId)
			{
				return p;
			}
		}
		return invalid();
	}

	AbstractPlayer AbstractPlayer::fromEndpointId(uint32_t id)
	{
		if (auto mgr = getNetworkPlayerMgrIfApplicable())
		{
			for (compactplayer_t p = 0; p < MAX_PLAYERS; p++)
			{
				if (mgr->Players[p] != nullptr
					&& mgr->Players[p]->endpoint_id == id
					)
				{
					return p;
				}
			}
		}
		return invalid();
	}

	AbstractPlayer AbstractPlayer::fromEndpointAddress(const rage::netSocketAddress& addr)
	{
		if (auto mgr = getNetworkPlayerMgrIfApplicable())
		{
			for (compactplayer_t p = 0; p < MAX_PLAYERS; p++)
			{
				if (mgr->Players[p] != nullptr
					&& p != g_player
					)
				{
					if (auto endpoint = mgr->Players[p]->getEndpoint())
					{
						if (endpoint->getRemoteAddress() == addr
							|| endpoint->relay_address.proxy_sock_addr == addr
							)
						{
							return p;
						}
					}
				}
			}
		}
		return invalid();
	}

	AbstractPlayer AbstractPlayer::getNextHost()
	{
		AbstractPlayer best_candidate = AbstractPlayer::invalid();
		const auto list = AbstractPlayer::listAll();
		if (list.size() >= 2)
		{
			const auto comp = host_queue_sort_algos[(*pointers::network_session)->host_queue_sort_algo];
			for (const auto& p : list)
			{
				if (!p.canBecomeHost())
				{
					continue;
				}
				if (!best_candidate.isValid() || comp(p, best_candidate))
				{
					best_candidate = p;
				}
			}
		}
		return best_candidate;
	}

	bool AbstractPlayer::exists() const
	{
		if (!isValid())
		{
			return false;
		}
		if (auto mgr = getNetworkPlayerMgrIfApplicable())
		{
			__try
			{
				return mgr->Players[p] != nullptr;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return false;
			}
		}
		return p == 0;
	}

	const char* AbstractPlayer::getUserName()
	{
		return g_hooking.spoof_name ? g_hooking.spoofed_name.c_str() : pointers::rlPresence_GamerPresences_0_GamerInfo->name;
	}

	std::string AbstractPlayer::getName() const
	{
		if (isValid())
		{
			if (*this == g_player)
			{
				return getUserName();
			}
			if (auto name = getOverrideName())
			{
				return name;
			}
			return filter_name_pretty(getUnoverriddenName());
		}
		return soup::ObfusString("InvalidPlayer").str();
	}

	std::string AbstractPlayer::getNameWithFlags() const
	{
		auto str = getName();
		if (std::u32string flags_str = getFlagsStrings(); !flags_str.empty())
		{
			str.append(StringUtils::utf16_to_utf8(CommandPlayer::flag_prefix));
			str.append(soup::unicode::utf32_to_utf8(flags_str));
			str.append(StringUtils::utf16_to_utf8(CommandPlayer::flag_suffix));
		}
		return str;
	}

	std::wstring AbstractPlayer::getNameW() const
	{
		return StringUtils::utf8_to_utf16(getName());
	}

	std::wstring AbstractPlayer::getNameW(bool with_flags) const
	{
		std::wstring name = getNameW();
		if (with_flags)
		{
			std::u32string flags_str = getFlagsStrings();
			if (!flags_str.empty())
			{
				name.append(CommandPlayer::flag_prefix);
				name.append(soup::unicode::utf32_to_utf16(flags_str));
				name.append(CommandPlayer::flag_suffix);
			}
		}
		return name;
	}

#define APPEND_FLAG(id, name) \
if (cmd->flags & (1 << id)) \
{ \
	const char32_t flag_char = CommandPlayer::flag_chars[id]; \
	if (flag_char != 0) \
	{ \
		flags_str.push_back(flag_char); \
	} \
}

	std::u32string AbstractPlayer::getFlagsStrings() const
	{
		std::u32string flags_str{};
		if (auto cmd = getCommand())
		{
			FOR_ALL_FLAGS(APPEND_FLAG);
		}
		return flags_str;
	}

	const char* AbstractPlayer::getRedundantName() const
	{
		return redundant_names.at(p).c_str();
	}

	uint32_t AbstractPlayer::getNameHash() const
	{
		return rage::atStringHash(getGamerInfo()->name);
	}

	const char* AbstractPlayer::getMaskName(const std::string& name)
	{
		soup::LcgRngInterface rng(rage::atStringHash(name));
		if (rng.coinflip())
		{
			return Names::female[rng.generate() % NAMES_FEMALE_SIZE];
		}
		return Names::male[rng.generate() % NAMES_MALE_SIZE];
	}

	void AbstractPlayer::spoofName() const
	{
		setOverrideName(getMaskName(getSessionName()));
	}

	void AbstractPlayer::unspoofName() const
	{
		clearOverrideName();
	}

	void AbstractPlayer::forceCommandRecreationIfNotUser() const
	{
		if (*this != g_player)
		{
			if (auto cmd = getCommand())
			{
				cmd->force_recreate = true;
			}
		}
	}

	void AbstractPlayer::updateNameOnBlip() const
	{
		HUD::SET_BLIP_COLOUR(ScriptGlobal(GLOBAL_BLIPS).at(p, 1).get<Blip>(), 4);
	}

	Vector3 AbstractPlayer::getBlipPos() const
	{
		const auto blip = p == g_player ? HUD::GET_MAIN_PLAYER_BLIP_ID() : ScriptGlobal(GLOBAL_BLIPS).at(p, 1).get<Blip>();
		return HUD::GET_BLIP_COORDS(blip);
	}

	std::string AbstractPlayer::getUnoverriddenName() const
	{
		if (auto player_info = getPlayerInfoNoFallback())
		{
			return player_info->gamer_info.name;
		}
		return soup::ObfusString("UndiscoveredPlayer").str();
	}

	const char* AbstractPlayer::getOverrideName() const
	{
		if (auto e = g_hooking.name_overrides.find(getPeerId()); e != g_hooking.name_overrides.end())
		{
			return e->second.c_str();
		}
		return nullptr;
	}

	void AbstractPlayer::setOverrideName(const std::string& name) const
	{
		uint64_t peer_id = getPeerId();
		if (auto e = g_hooking.name_overrides.find(peer_id); e != g_hooking.name_overrides.end())
		{
			e->second = name;
		}
		else
		{
			g_hooking.name_overrides.emplace(peer_id, name);
		}
	}

	void AbstractPlayer::clearOverrideName() const
	{
		if (auto e = g_hooking.name_overrides.find(getPeerId()); e != g_hooking.name_overrides.end())
		{
			g_hooking.name_overrides.erase(e);
		}
	}

	std::string AbstractPlayer::getSessionName() const
	{
		if (*this == g_player)
		{
#if false // Ignoring name spoofing
			return getUserName();
#else
			return pointers::rlPresence_GamerPresences_0_GamerInfo->name;
#endif
		}
		return getUnoverriddenName();
	}

	std::wstring AbstractPlayer::getSessionNameForUser() const
	{
		if (isInScope(hide_unfiltered_name))
		{
			return LANG_GET_W("RDCTD");
		}
		return StringUtils::utf8_to_utf16(getSessionName());
	}

	const rage::rlGamerHandle& AbstractPlayer::getGamerHandle() const
	{
		return getGamerInfo()->getHandle();
	}

	int64_t AbstractPlayer::getRockstarId() const
	{
		return getGamerHandle().rockstar_id;
	}

	rage::netSocketAddress AbstractPlayer::getAddress() const
	{
		auto connect_addr = getConnectAddress();
		/*if (CidrSupernets::_private.contains(connect_addr.ip.value))
		{
			return getOnlineAddress();
		}*/
		if (connect_addr.ip == -1)
		{
			if (auto peer = IceQueen::findPeer(getStartupId()))
			{
				connect_addr = peer->public_addr;
			}
		}
		return connect_addr;
	}

	rage::netSocketAddress AbstractPlayer::getConnectAddress() const
	{
		if (*this == g_player)
		{
			return getGamerInfo()->peer.address.local_only_online;
		}
		if (auto endpoint = getEndpoint())
		{
			return endpoint->getRemoteAddress();
		}
		return {};
	}

	rage::netSocketAddress AbstractPlayer::getLanAddress() const
	{
		if (*this == g_player)
		{
			return getGamerInfo()->peer.address.local_only_lan;
		}
		if (auto peer = IceQueen::findPeer(getStartupId()))
		{
			return peer->lan_addr;
		}
		return {};
	}

	std::string AbstractPlayer::getIpAddressForUserA() const
	{
		if (isInScope(hide_ip))
		{
			return LANG_GET("RDCTD");
		}
		auto addr = getAddress();
		if (addr.ip == -1)
		{
			return LANG_GET("NA");
		}
		return addr.ip.toString<std::string>();
	}

	std::wstring AbstractPlayer::getIpAddressForUserW() const
	{
		if (isInScope(hide_ip))
		{
			return LANG_GET_W("RDCTD");
		}
		auto addr = getAddress();
		if (addr.ip == -1)
		{
			return LANG_GET_W("NA");
		}
		return addr.ip.toString<std::wstring>();
	}

	std::string AbstractPlayer::getConnectedViaForPlayerHelpText() const
	{
		if (auto ep = getEndpoint())
		{
			switch (ep->GetAddress().m_Type)
			{
			case rage::netAddress::TYPE_RELAY_SERVER:
				return LANG_GET("CONVIA_RLY_S");

			case rage::netAddress::TYPE_PEER_RELAY:
				return LANG_FMT("CONVIA_PLY_S", AbstractPlayer::fromEndpointAddress(ep->GetAddress().proxy_sock_addr).getName());
			}
		}
		return {};
	}

	std::wstring AbstractPlayer::getConnectedViaForPlayerInfoValue() const
	{
		if (auto ep = getEndpoint())
		{
			switch (ep->GetAddress().m_Type)
			{
			case rage::netAddress::TYPE_RELAY_SERVER:
				return LANG_GET_W("CONVIA_RLY");

			case rage::netAddress::TYPE_PEER_RELAY:
				return AbstractPlayer::fromEndpointAddress(ep->GetAddress().proxy_sock_addr).getNameW();
			}
		}
		return LANG_GET_W("CONVIA_P2P");
	}

	std::string AbstractPlayer::getCountry() const
	{
		return getAddress().ip.getGeoIpField(GEOIP_COUNTRY);
	}

	Label AbstractPlayer::getLanguageName() const
	{
		static const Label names[] = {
			LOC("LANG_EN"),
			LOC("LANG_FR"),
			LOC("LANG_DE"),
			LOC("LANG_IT"),
			LOC("LANG_ES"),
			LOC("LANG_BR"),
			LOC("LANG_PL"),
			LOC("LANG_RU"),
			LOC("LANG_KR"),
			LOC("LANG_TW"),
			LOC("LANG_JP"),
			LOC("LANG_MX"),
			LOC("LANG_CN"),
		};

		if (const auto language = getLanguage(); language >= 0 && language <= 12)
		{
			return names[language];
		}

		return LOC("NA");
	}

	int AbstractPlayer::getLanguage() const
	{
		if (*this == g_player)
		{
			return LOCALIZATION::GET_CURRENT_LANGUAGE();
		}

		return ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(*this, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG + GLOBAL_ORG_LANG).get<int>();
	}

	bool AbstractPlayer::isUsingController() const
	{
		if (*this == g_player)
		{
			return PAD::IS_USING_KEYBOARD_AND_MOUSE(0) == FALSE;
		}

		return ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(*this, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_CONTROL_TYPE).get<int>() == 2;
	}

	rage::netPeerId AbstractPlayer::getPeerId() const
	{
		return getGamerInfo()->peer.id;
	}

	AbstractPlayer::host_queue_sort_algo_t AbstractPlayer::host_queue_sort_algos[] = {
		[](const AbstractPlayer& a, const AbstractPlayer& b) -> bool { return a.getRockstarId() < b.getRockstarId(); },
		[](const AbstractPlayer& a, const AbstractPlayer& b) -> bool { return a.getRockstarId() > b.getRockstarId(); },
		[](const AbstractPlayer& a, const AbstractPlayer& b) -> bool { return a.getNameHash() < b.getNameHash(); },
		[](const AbstractPlayer& a, const AbstractPlayer& b) -> bool { return a.getNameHash() > b.getNameHash(); }
	};

	compactplayer_t AbstractPlayer::getHostQueuePosition() const
	{
		if (canBecomeHost())
		{
			auto list = AbstractPlayer::listAll();
			if (list.size() > 1)
			{
				const auto comp = host_queue_sort_algos[(*pointers::network_session)->host_queue_sort_algo];
				compactplayer_t pos = 1;
				for (const auto& p : list)
				{
					if (p.canBecomeHost() && comp(p, *this))
					{
						++pos;
					}
				}
				return pos;
			}
		}
		return 0;
	}

	rage::netPeerId AbstractPlayer::getStartupId() const
	{
		if (auto e = getEndpoint())
		{
			return e->GetPeerId();
		}

		// Fallback for local user.
		return getGamerInfo()->peer.address.startup_id;
	}

	int64_t AbstractPlayer::getAccountId() const
	{
		if (auto cngp = getCNetGamePlayer())
		{
			return cngp->m_PlayerAccountId;
		}
		return 0;
	}

	uint32_t AbstractPlayer::getMacId() const
	{
		return getStartupId() >> 32;
	}

	CNetGamePlayer* AbstractPlayer::getCNetGamePlayer() const
	{
		if (isValid())
		{
			if (auto mgr = getNetworkPlayerMgrIfApplicable())
			{
				return mgr->Players[p];
			}
		}
		return nullptr;
	}

	CPlayerInfo* AbstractPlayer::getPlayerInfoNoFallback() const
	{
		if (isValid())
		{
			if (*this == g_player)
			{
				return g_player_ped.getPlayerInfo();
			}
			if (auto mgr = getNetworkPlayerMgrIfApplicable())
			{
				if (auto player = mgr->Players[p])
				{
					return player->player_info;
				}
			}
		}
		return nullptr;
	}

	CPlayerInfo* AbstractPlayer::getPlayerInfo() const
	{
		if (isValid())
		{
			if (auto mgr = getNetworkPlayerMgrIfApplicable())
			{
				if (auto player = mgr->Players[p])
				{
					return player->player_info;
				}
			}
		}
		return g_player_ped.getPlayerInfo();
	}

	rage::rlGamerInfo* AbstractPlayer::getGamerInfoNoFallback() const
	{
		if (p == g_player)
		{
			return pointers::rlPresence_m_ActingGamerInfo;
		}
		auto* player_info = getPlayerInfoNoFallback();
		if (player_info != nullptr)
		{
			return &player_info->gamer_info;
		}
		return nullptr;
	}

	rage::rlGamerInfo* AbstractPlayer::getGamerInfo() const
	{
		if (p != g_player)
		{
			auto* player_info = getPlayerInfo();
			if (player_info != nullptr)
			{
				return &player_info->gamer_info;
			}
		}
		return pointers::rlPresence_m_ActingGamerInfo;
	}

	rage::netPeerAddress* AbstractPlayer::getPeer() const
	{
		return &getGamerInfo()->peer.address;
	}

	rage::netEndpoint* AbstractPlayer::getEndpoint() const
	{
		if (auto np = getCNetGamePlayer())
		{
			return np->getEndpoint();
		}
		return nullptr;
	}

	uint32_t AbstractPlayer::getEndpointId() const
	{
		if (auto np = getCNetGamePlayer())
		{
			return np->endpoint_id;
		}
		return -1;
	}

	int32_t AbstractPlayer::getCxnId(const uint32_t channel_id) const
	{
		auto endpoint = getEndpoint();
		if (endpoint && endpoint->channel_cxns[channel_id])
		{
			return endpoint->channel_cxns[channel_id]->id;
		}
		return -1;
	}

	rage::rlClanMembershipData* AbstractPlayer::getActiveClanMembership() const
	{
		if (*this != g_player)
		{
			if (auto cngp = getCNetGamePlayer())
			{
				return &cngp->clan_membership;
			}
		}
		return pointers::rage_rlClan_GetPrimaryMembership(0);
	}

	CPed* AbstractPlayer::getCPedPtr() const
	{
		auto* const player_info = getPlayerInfoNoFallback();
		if (player_info != nullptr)
		{
			return player_info->m_ped;
		}
		return nullptr;
	}

	bool AbstractPlayer::hasPed() const
	{
		return getCPedPtr() != nullptr;
	}

	AbstractEntity AbstractPlayer::getPed() const
	{
		return AbstractEntity::get(getCPedPtr());
	}

	AbstractEntity AbstractPlayer::getVehicle() const
	{
		return getPed().getVehicle();
	}

	AbstractEntity AbstractPlayer::getVehicle(Click& click) const
	{
		auto veh = getVehicle();
		if (!veh.isValid())
		{
			click.setResponse(LIT(LANG_FMT("PLYNVEH", getName())));
		}
		return veh;
	}

	int32_t AbstractPlayer::getWantedLevel() const
	{
		if (isValid())
		{
			return PLAYER::GET_PLAYER_WANTED_LEVEL(p);
		}
		return 0;
	}

	void AbstractPlayer::incRemoteWantedLevel(int32_t wanted_level) const
	{
		if (auto cngp = getCNetGamePlayer())
		{
			pointers::CAlterWantedLevelEvent_Trigger(cngp, wanted_level, 0, (compactplayer_t)g_player);
		}
	}

	void AbstractPlayer::clearWantedLevel() const
	{
		uint64_t args[] = {
			Util::i32_to_u64_trickery(SE_CLEAR_WANTED),
			g_player,
			(uint64_t)*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_SEVERIFY).as<int*>()
		};
		TSE(1 << p, args);
	}

	void AbstractPlayer::setWantedLevel(int32_t wanted_level) const
	{
		if (!isValid())
		{
			return;
		}
		if (*this != g_player)
		{
			if (getWantedLevel() != wanted_level)
			{
				if (getWantedLevel() < wanted_level)
				{
					incRemoteWantedLevel(wanted_level);
				}
				else
				{
					clearWantedLevel();
					if (wanted_level != 0)
					{
						FiberPool::queueJob([p{ *this }, wanted_level]
						{
							Script::current()->yield(200);
							p.incRemoteWantedLevel(wanted_level);
						});
					}
				}
			}
			return;
		}
		g_player_ped.getPlayerInfo()->m_Wanted.m_bTimeCounting = (wanted_level != 0);
		g_player_ped.getPlayerInfo()->m_Wanted.m_WantedLevelOld = (eWantedLevel)wanted_level;
		g_player_ped.getPlayerInfo()->m_Wanted.m_WantedLevel = (eWantedLevel)wanted_level;
	}

	v3 AbstractPlayer::getPos() const
	{
		if (*this == g_player)
		{
			// Using g_player_ent reduces "drifting away" when CommandSpawnVehicle is used a lot.
			return g_player_ent.getPos();
		}

		auto ped = getPed();
		v3 pos = ped.getPos();
		if (auto net_obj = ped.getNetObject())
		{
			if (net_obj->m_NetBlender)
			{
				pos.z = static_cast<rage::netBlenderLinInterp*>(net_obj->m_NetBlender)->m_PredictionData.m_SnapshotPresent.m_Position.z;
			}
		}
		return pos;
	}

	void AbstractPlayer::tickAutoHeal() const
	{
		AbstractEntity ped = getPed();
		if (ped.isValid())
		{
			if (ped.getHealth() < ped.getMaxHealth())
			{
				givePickupReward(ATSTRINGHASH("REWARD_HEALTH"));
			}
			if (ped.getArmour() < 50.0f)
			{
				auto* cmd = getCommand();
				if (cmd != nullptr && GET_MILLIS_SINCE(cmd->discovery) > 40000)
				{
					givePickupReward(ATSTRINGHASH("REWARD_ARMOUR"));
				}
			}
		}
	}

	void AbstractPlayer::givePickupReward(hash_t reward) const
	{
		pointers::create_and_send_pickup_rewards_event(1 << p, reward);
	}

	void AbstractPlayer::sendFreeze() const
	{
		// This freezes the player for a few seconds, but does not kick them from vehicles.
		uint64_t data[] = {
			Util::i32_to_u64_trickery(SE_ENTER_WAREHOUSE),
			g_player,
			0,
			1,
			0
		};
		TSE(1 << p, data);
	}

	CommandPlayer* AbstractPlayer::getCommand() const noexcept
	{
		return isValid() ? g_gui.player_commands[p] : nullptr;
	}

	CommandList* AbstractPlayer::getCommandAsList() const noexcept
	{
		return getCommand();
	}

	bool AbstractPlayer::satisfiesAnyOf(playerflag_t flags) const noexcept
	{
		const auto cmd = getCommand();
		return cmd != nullptr
			&& (cmd->flags & flags) != 0
			;
	}

	bool AbstractPlayer::satisfiesAnyOf(bool is_friend, bool is_crew_member, bool is_stand_user, bool is_org_member, bool is_modder, bool is_likely_modder) const
	{
		return (is_friend && isFriend())
			|| (is_crew_member && isCrewMember())
			|| (is_stand_user && isStandUser())
			|| (is_org_member && inOurOrg())
			|| (is_modder && hasFlag(FLAG_MOD))
			|| (is_likely_modder && hasFlag(FLAG_LIKELY_MODDER))
			;
	}

	bool AbstractPlayer::isFrozen() const
	{
		const CommandPlayer* const player_command = getCommand();
		return player_command == nullptr ? false : ((CommandToggleNoCorrelation*)player_command->recursivelyResolveChildByMenuName(LOC("FRZ")))->m_on;
	}

	void AbstractPlayer::freeze(const bool toggle) const
	{
		const CommandPlayer* const player_command = getCommand();
		if (player_command != nullptr)
		{
			auto* const freeze_command = (CommandToggleNoCorrelation*)player_command->recursivelyResolveChildByMenuName(LOC("FRZ"));
			if (freeze_command->m_on != toggle)
			{
				Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
				freeze_command->onClick(click);
			}
		}
	}

	void AbstractPlayer::unfreeze() const
	{
		freeze(false);
	}

	void AbstractPlayer::kill() const
	{
		SOUP_IF_LIKELY (auto cmd = getCommand())
		{
			Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
			((CommandList*)cmd->resolveChildByMenuName(LOC("TROLL")))->resolveChildByMenuName(LOC("PLYKLL"))->onClick(click);
		}
	}

	playertype_t AbstractPlayer::getType() const
	{
		if (*this == g_player)
		{
			return PlayerType::SELF;
		}
		if (isFriend())
		{
			return PlayerType::FRIEND;
		}
		return PlayerType::STRANGER;
	}

	void AbstractPlayer::onFlowEvent(flowevent_t event, playertype_t playertype) const
	{
		if (playertype != PlayerType::SELF)
		{
			switch (event)
			{
			case FlowEvent::NE_DISARM:
			case FlowEvent::SE_CRASH:
			case FlowEvent::SE_KICK:
			case FlowEvent::SE_CEOKICK:
			case FlowEvent::SE_INFLOAD:
			case FlowEvent::SE_DISABLEDRIVINGVEHICLES:
				setAttacked();
				break;
			}
		}
	}

	void AbstractPlayer::setAttacked() const
	{
		auto* command = getCommand();
		if (command != nullptr)
		{
			command->last_attack = get_current_time_millis();
			if (!(command->flags & (1 << FLAG_ATTACKED)))
			{
				command->flags |= (1 << FLAG_ATTACKED);
				command->updateFlags(*this);
			}
		}
	}

	floweventreaction_t AbstractPlayer::getReactions(flowevent_t event, bool event_is_being_dispatched, bool alternative_self_handling) const
	{
		return getReactions(event, getType(), event_is_being_dispatched, alternative_self_handling);
	}

	floweventreaction_t AbstractPlayer::getReactions(flowevent_t event, playertype_t playertype, bool event_is_being_dispatched, bool alternative_self_handling) const
	{
		if (alternative_self_handling && playertype == PlayerType::SELF && !FlowEvent::can_be_self(event))
		{
			playertype = PlayerType::FRIEND;
		}
		if (event_is_being_dispatched)
		{
			onFlowEvent(event, playertype);
		}
		if (!g_gui.doesRootStateAllowProtections())
		{
			return 0;
		}
		floweventreaction_t reactions = g_hooking.flow_event_reactions[event].getReactions(playertype);
		if (FlowEvent::is_net_or_script_event(event))
		{
			reactions |= g_hooking.flow_event_reactions[FlowEvent::NE_ANY].getReactions(playertype);
			if (isValid())
			{
				reactions |= g_hooking.player_net_event_reactions[p];
				if (isInTimeout())
				{
					reactions |= REACTION_BLOCK;
				}
			}
			if (FlowEvent::is_ac_event(event))
			{
				reactions &= ~REACTION_BLOCK;
			}
		}
		else if (FlowEvent::is_sync_in(event))
		{
			reactions |= g_hooking.flow_event_reactions[FlowEvent::SYNCIN_ANY].getReactions(playertype);
			if (isValid())
			{
				reactions |= g_hooking.player_sync_in_reactions[p];
				if (isInTimeout())
				{
					reactions |= REACTION_BLOCK;
				}
			}
		}
		else if (FlowEvent::is_sync_out(event))
		{
			reactions |= g_hooking.player_sync_out_reactions[MAX_PLAYERS];
			if (isValid())
			{
				reactions |= g_hooking.player_sync_out_reactions[p];
				if ((g_hooking.block_outgoing_syncs_to_host && isHost())
					|| isInTimeout()
					)
				{
					reactions |= REACTION_BLOCK;
				}
			}
		}
		else if (FlowEvent::is_modder_detection(event))
		{
			reactions |= g_hooking.flow_event_reactions[FlowEvent::MOD_ANY].getReactions(playertype);
		}
		return reactions;
	}

	void AbstractPlayer::addReactions(floweventreaction_t& reactions, flowevent_t event) const
	{
		const playertype_t playertype = getType();
		onFlowEvent(event, playertype);
		reactions |= getReactions(event, playertype);
	}

	floweventreaction_t AbstractPlayer::getReactionsRaw(flowevent_t event, playertype_t playertype)
	{
		return g_hooking.flow_event_reactions[event].getReactions(playertype);
	}

	floweventreaction_t AbstractPlayer::triggerDetection(flowevent_t type, std::string&& extra_data, uint8_t severity) const
	{
		floweventreaction_t reactions = getReactions(type);
		if (reactions != 0)
		{
			applyReactions(reactions, NuancedEventWithExtraData(type, std::move(extra_data), severity));
		}
		return reactions;
	}

	floweventreaction_t AbstractPlayer::getAndApplyReactionsIn(flowevent_t type, const std::string& extra_data) const
	{
		floweventreaction_t reactions = getReactions(type);
		if (reactions != 0)
		{
			applyReactionsIn(reactions, type, extra_data);
		}
		return reactions;
	}

	void AbstractPlayer::applyReactions(floweventreaction_t reactions) const
	{
		CommandPlayer* const command = getCommand();
		if (command != nullptr)
		{
			command->applyReactions(reactions);
		}
	}

	void AbstractPlayer::applyReactions(floweventreaction_t reactions, flowevent_t event) const
	{
		if (auto cmd = getCommand())
		{
			cmd->applyReactions(reactions, NuancedEvent(event));
		}
	}

	void AbstractPlayer::applyReactions(floweventreaction_t reactions, flowevent_t event, const Label& name) const
	{
		if (auto cmd = getCommand())
		{
			cmd->applyReactions(reactions, NuancedEvent(event), name);
		}
	}

	void AbstractPlayer::applyReactions(floweventreaction_t reactions, std::vector<flowevent_t>&& types, Label&& name) const
	{
		applyReactions(reactions, EventAggregation(std::move(types), std::move(name)));
	}

	void AbstractPlayer::applyReactions(floweventreaction_t reactions, EventAggregation&& event, uint8_t severity) const
	{
		if (auto cmd = getCommand())
		{
			cmd->applyReactions(reactions, NuancedEvent(event.getCanonicalType(), severity), std::move(event.name), event.extra_data);
		}
	}

	void AbstractPlayer::applyReactions(floweventreaction_t reactions, NuancedEventWithExtraData&& event) const
	{
		if (auto cmd = getCommand())
		{
			cmd->applyReactions(reactions, std::move(event));
		}
	}

	void AbstractPlayer::applyReactions(EventLabel&& label, std::vector<flowevent_t>&& types, floweventreaction_t reactions, hash_t format_string_key, uint8_t severity) const
	{
		if (auto tf = flow_event_reactions_to_logger_toast_flags(reactions))
		{
			std::string str = fmt::format(fmt::runtime(Lang::get(format_string_key)), FMT_ARG("event", label.getName()), FMT_ARG("player", getName()));
			if (label.hasNonRawName())
			{
				str.append(": ").append(label.raw_name.getLocalisedUtf8());
			}
			Util::toast(std::move(str), tf);
		}
		if (auto tf = flow_event_reactions_to_toast_flags(reactions))
		{
			std::string toast = fmt::format(fmt::runtime(Lang::get(format_string_key)), FMT_ARG("event", label.getName()), FMT_ARG("player", getName()));
			augmentReactionToast(toast, reactions);
			Util::toast(std::move(toast), tf);
		}
		applyReactions(reactions, EventAggregation(std::move(types), std::move(label.name)), severity);
	}

	void AbstractPlayer::applyReactionsIn(EventLabel&& label, std::vector<flowevent_t>&& types, floweventreaction_t reactions, uint8_t severity) const
	{
		return applyReactions(std::move(label), std::move(types), reactions, ATSTRINGHASH("PTX_NET_R_T_T"), severity);
	}

	void AbstractPlayer::applyReactionsIn(floweventreaction_t reactions, flowevent_t type, const std::string& extra_data) const
	{
		return applyReactionsIn(FlowEvent::getName(type, extra_data), { type }, reactions);
	}

	void AbstractPlayer::applyReactionsOut(EventLabel&& label, std::vector<flowevent_t>&& types, floweventreaction_t reactions) const
	{
		return applyReactions(std::move(label), std::move(types), reactions, ATSTRINGHASH("RCT_T_OUT"));
	}

	void AbstractPlayer::augmentReactionToast(std::string& toast, floweventreaction_t reactions)
	{
		Label reaction = NOLABEL;
		if (((reactions & REACTION_BLACKLIST_KICK) && g_player.isHost())
			|| (reactions & REACTION_KICK)
			|| (reactions & REACTION_LL_KICK)
			)
		{
			reaction = LOC("REACT_T_K");
		}
		else if (reactions & REACTION_CRASH)
		{
			reaction = LOC("REACT_T_C");
		}
		else if (reactions & REACTION_TIMEOUT)
		{
			reaction = LOC("REACT_T_T");
		}
		if (!reaction.empty())
		{
			toast = LANG_FMT("REACT_T", FMT_ARG("toast", toast), FMT_ARG("reaction", reaction.getLocalisedUtf8()));
		}
	}

	bool AbstractPlayer::hasFlag(playerflag_t flag) const
	{
		CommandPlayer* const command = getCommand();
		if (command != nullptr)
		{
			return (command->flags >> flag) & 1;
		}
		return false;
	}

	void AbstractPlayer::blockIncomingNetEvents(ThreadContext thread_context) const
	{
		CommandPlayer* const command = getCommand();
		if (command != nullptr)
		{
			command->blockIncomingNetEvents(thread_context);
		}
	}

	floweventreaction_t AbstractPlayer::getOutgoingSyncReactions() const
	{
		floweventreaction_t reactions = (g_hooking.player_sync_out_reactions[p] | g_hooking.player_sync_out_reactions[MAX_PLAYERS]);
		if (isInTimeout())
		{
			reactions |= REACTION_BLOCK;
		}
		return reactions;
	}

	bool AbstractPlayer::blockingOutgoingSyncs() const
	{
		return getOutgoingSyncReactions() & REACTION_BLOCK;
	}

	bool AbstractPlayer::isInTimeout() const noexcept
	{
		return isValid() && g_hooking.player_timeouts[p] != 0;
	}

	void AbstractPlayer::putInTimeout(ThreadContext thread_context) const
	{
		if (auto cmd = getCommand())
		{
			cmd->putInTimeout(thread_context);
		}
	}

	template <typename T>
	[[nodiscard]] static T getGlobalStat(const AbstractPlayer& p, uint32_t offset)
	{
		if (p.isValid() && is_session_started())
		{
			return ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(offset).get<T>();
		}
		return {};
	}

	bool AbstractPlayer::areStatsReady() const
	{
		return ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_FAVOURITE_STATION).get<Hash>() != 0;
	}

	bool AbstractPlayer::areStatsBusted() const
	{
		// As of 3095, some stats are not synched correctly for some players. This can also be observed in the in-game player list.
		return getRank() == 1
			&& getRp() < 800
			&& getKd() == 0.01f
			;
	}

	int AbstractPlayer::getRp() const
	{
		return getGlobalStat<int>(*this, GLOBAL_PLAYER_STAT_RP);
	}

	int AbstractPlayer::getRank() const
	{
		return getGlobalStat<int>(*this, GLOBAL_PLAYER_STAT_RANK);
	}

	unsigned int AbstractPlayer::getMoney() const
	{
		return getGlobalStat<unsigned int>(*this, GLOBAL_PLAYER_STAT_MONEY);
	}

	unsigned int AbstractPlayer::getWallet() const
	{
		return getGlobalStat<unsigned int>(*this, GLOBAL_PLAYER_STAT_WALLET);
	}

	unsigned int AbstractPlayer::getBank() const
	{
		return getMoney() - getWallet();
	}

	float AbstractPlayer::getKd() const
	{
		return getGlobalStat<float>(*this, GLOBAL_PLAYER_STAT_KD);
	}

	int AbstractPlayer::getKills() const
	{
		return getGlobalStat<int>(*this, GLOBAL_PLAYER_STAT_KILLS);
	}

	int AbstractPlayer::getDeaths() const
	{
		return getGlobalStat<int>(*this, GLOBAL_PLAYER_STAT_DEATHS);
	}

	bool AbstractPlayer::isOtr() const
	{
		return isValid()
			&& is_session_started()
			&& ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_OTR1).get<BOOL>();
	}

	bool AbstractPlayer::isOutOfSight() const
	{
		return isValid()
			&& is_session_started()
			&& (ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_IMANI_OUTOFSIGHT_FLAGS).get<int>() & (1 << eOutOfSightFlags::kOutOfSightActive));
	}

	bool AbstractPlayer::isInInterior() const
	{
		if (isValid())
		{
			if (is_session_started())
			{
				return (getApartmentId() != -1)
					|| (getShopId() != -1)
					|| (getInteriorId() != 0 && getPos().z < -20.0f) // getInteriorId() returns pseudo-interiors too, like tunnels. These are above ground, unlike proper interiors that are under the map.
					;
			}
			else
			{
				return INTERIOR::GET_INTERIOR_FROM_ENTITY(getPed()) != 0;
			}
		}
		return false;
	}

	bool AbstractPlayer::isHost() const noexcept
	{
		return host == p;
	}

	bool AbstractPlayer::canBecomeHost() const noexcept
	{
		return !isHost();
	}

	bool AbstractPlayer::isNextHost() const
	{
		return *this == AbstractPlayer::getNextHost();
	}

	bool AbstractPlayer::isScriptHost() const
	{
		return script_host == p;
	}

	bool AbstractPlayer::isFriend() const
	{
		CommandPlayer* const command = getCommand();
		if (command == nullptr)
		{
			return false;
		}
		return command->is_friend;
	}

	bool AbstractPlayer::isCrewMember() const
	{
		CommandPlayer* const command = getCommand();
		if (command == nullptr)
		{
			return false;
		}
		return command->is_crew_member;
	}

	bool AbstractPlayer::isStandUser() const
	{
		CommandPlayer* const command = getCommand();
		return command != nullptr
			&& command->isStandUser()
			;
	}

	bool AbstractPlayer::isStandDevDbg() const
	{
		CommandPlayer* const command = getCommand();
		return command != nullptr
			&& (command->dp_flags & DPFLAG_DEVDBG)
			;
	}

	bool AbstractPlayer::isRockstarAdmin() const
	{
		CommandPlayer* const command = getCommand();
		if (command != nullptr)
		{
			return command->classifier.classify() == PlayerClassifier::ADMIN;
		}
		return false;
	}

	bool AbstractPlayer::isAnyRockstarAdminInSession()
	{
		for (const auto& p : AbstractPlayer::listExcept(true))
		{
			if (p.isRockstarAdmin())
			{
				return true;
			}
		}
		return false;
	}

	bool AbstractPlayer::isTyping() const
	{
		return is_session_started()
			&& ScriptGlobal(GLOBAL_PLAYERSTATUS1_BASE).at(GLOBAL_PLAYERSTATUS1_TAG).at(GLOBAL_PLAYERSTATUS1_TAG_TYPING / 32, 33).at(p, 1).get<uint32_t>() & (1 << (GLOBAL_PLAYERSTATUS1_TAG_TYPING % 32))
			;
	}

	float* AbstractPlayer::mentalState() const
	{
		return ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_MENTALSTATE).as<float*>();
	}

	float AbstractPlayer::getMentalState() const
	{
		return ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_MENTALSTATE).get<float>();
	}

	int AbstractPlayer::getInteriorId() const
	{
		if (isValid() && is_session_started())
		{
			return ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_interiorCurrent).get<int>();
		}
		return 0;
	}

	int AbstractPlayer::getShopId() const
	{
		if (isValid() && is_session_started())
		{
			return ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_iCurrentShop).get<int>();
		}
		return -1;
	}

	int AbstractPlayer::getApartmentId() const
	{
		return getGlobalStat<int>(*this, GLOBAL_PLAYER_STAT_APTID);
	}

	AbstractModel AbstractPlayer::getCurrentVehicleModel() const
	{
		if (isValid())
		{
			if (auto veh = getVehicle(); veh.isValid())
			{
				return veh.getModel();
			}
			if ((ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_VEHBITSET1).get<int>() >> GLOBAL_PLAYER_STAT_VEHBITSET1_RCBANDITO) & 1)
			{
				return ATSTRINGHASH("rcbandito");
			}
			if ((ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_VEHBITSET2).get<int>() >> GLOBAL_PLAYER_STAT_VEHBITSET2_MINITANK) & 1)
			{
				return ATSTRINGHASH("minitank");
			}
			if (auto blipVehType = ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_BLIPVEHTYPE).get<int>())
			{
				if (auto sprite = BlipUtil::vehicleTypeToSprite(blipVehType))
				{
					if (auto model = BlipUtil::spriteToModel(blipVehType))
					{
						return model;
					}
				}
			}
		}
		return 0;
	}

	bool AbstractPlayer::isInVehicle() const
	{
		return getCurrentVehicleModel() != 0
			|| PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(*this)
			;
	}

	bool AbstractPlayer::isUsingRcVehicle() const
	{
		return (ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_VEHBITSET1).get<int>() & (1 << GLOBAL_PLAYER_STAT_VEHBITSET1_RCBANDITO))
			|| (ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_VEHBITSET2).get<int>() & (1 << GLOBAL_PLAYER_STAT_VEHBITSET2_MINITANK))
			|| (ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_VEHBITSET3).get<int>() & (1 << GLOBAL_PLAYER_STAT_VEHBITSET3_RCPV))
			;
	}

	bool AbstractPlayer::isBlockingRequestControl() const
	{
		if (auto cmd = getCommand())
		{
			return cmd->classifier.hasEvent(FlowEvent::MOD_NONET);
		}
		return false;
	}

	bool AbstractPlayer::isInStore() const
	{
		switch (getInteriorId()) // Sourced from CommandAutoDriveAction. Same order.
		{
		case 153857: // Ammunation
		case 140289: // Ammunation
		case 180481: // Ammunation
		case 176385: // Ammunation
		case 200961: // Ammunation
		case 178689: // Ammunation
		case 175617: // Ammunation
		case 164609: // Ammunation
		case 137729: // Ammunation
		case 248065: // Ammunation
		case 140801: // Clothes
		case 235265: // Clothes
		case 165633: // Clothes
		case 166145: // Clothes
		case 169217: // Clothes
		case 171265: // Clothes
		case 198145: // Clothes
		case 137217: // Clothes
		case 201473: // Clothes
		case 202497: // Clothes
		case 179713: // Clothes
		case 176129: // Clothes
		case 175361: // Clothes
		case 140033: // Tats
		case 171521: // Tats
		case 176897: // Tats
		case 251137: // Tats
		case 180737: // Tats
		case 199425: // Tats
		case 155905: // Salon
		case 199937: // Salon
		case 171009: // Salon
		case 140545: // Salon
		case 198657: // Salon
		case 165377: // Salon
		case 180225: // Salon
		case 198401: // Store
		case 139777: // Store
		case 170753: // Store
		case 183809: // Store
		case 167937: // Store
		case 155649: // Store
		case 175873: // Store
		case 176641: // Store
		case 177153: // Store
		case 204801: // Store
		case 178945: // Store
		case 199169: // Store
		case 184065: // Store
		case 200449: // Store
		case 196865: // Store
		case 203265: // Store
		case 175105: // Store
		case 154113: // Store
		case 168449: // Store
			return true;

		default:
			return false;
		}
	}

	bool AbstractPlayer::inSameInterior(AbstractPlayer& other, std::optional<int> id) const noexcept
	{
		if (isInInterior())
		{
			return id.has_value()
				? (getInteriorId() == id.value() && other.getInteriorId() == id.value())
				: (getInteriorId() == other.getInteriorId())
				;
		}

		return false;
	}

	std::optional<int> AbstractPlayer::getBounty() const
	{
		if (hasBounty())
		{
			return getBountyValue();
		}
		return std::nullopt;
	}

	bool AbstractPlayer::hasBounty() const
	{
		return isValid()
			&& is_session_started()
			&& ScriptGlobal(GlobalServerBD_FM_currentBounties).at(p, GlobalServerBD_FM_currentBounties_size).get<BOOL>()
			;
	}

	int AbstractPlayer::getBountyValue() const
	{
		return ScriptGlobal(GlobalServerBD_FM_currentBounties).at(p, GlobalServerBD_FM_currentBounties_size).at(1).get<int>();
	}

	bool AbstractPlayer::isInAnOrg() const noexcept
	{
		return getBoss().isValid();
	}

	AbstractPlayer AbstractPlayer::getBoss() const noexcept
	{
		if (!is_session_started())
		{
			return MAX_PLAYERS;
		}
		return ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_BOSS).get<player_t>();
	}

	bool AbstractPlayer::isBoss() const noexcept
	{
		return getBoss() == *this;
	}

	int AbstractPlayer::getOrgType() const noexcept
	{
		return isValid()
			? ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_TYPE).get<int>()
			: -1
			;
	}

	bool AbstractPlayer::inOurOrg() const noexcept
	{
		const auto our_boss = g_player.getBoss();
		return our_boss.isValid()
			&& isValid()
			&& getBoss() == our_boss
			;
	}

	bool AbstractPlayer::isInMc() const noexcept
	{
		return getOrgType() == 1;
	}

	const char* AbstractPlayer::getOrgName() const noexcept
	{
		return ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_NAME).get<const char*>();
	}

	void AbstractPlayer::setOrgName(const char* name) const noexcept
	{
		setOrgName(isInMc(), name);
	}

	void AbstractPlayer::setOrgName(bool is_mc, const char* name) const noexcept
	{
		strcpy(ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_NAME).as<char*>(), name);
		strcpy(ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(is_mc ? GLOBAL_ORG_NAME_MC_COPY : GLOBAL_ORG_NAME_CEO_COPY).as<char*>(), name);
	}

	int AbstractPlayer::getColourSlot() const noexcept
	{
		auto boss = getBoss();
		if(boss.isValid())
		{
			return ScriptGlobal(GLOBAL_CEO_COLOUR_BOSS_TO_SLOT).at(boss, 1).get<int>();
		}
		return -1;
	}

	bool AbstractPlayer::isBeast() const
	{
		if (auto htb = GtaThread::fromHash(ATSTRINGHASH("am_hunt_the_beast")))
		{
			return *ScriptLocal(htb, LOCAL_HTB_BEAST).as<int*>() == p;
		}
		return false;
	}

	bool AbstractPlayer::inCayo() const
	{
		return ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_CAYOPERICOFLAGS).get<int>() & (1 << HITBS__ON_ISLAND);
	}

	bool AbstractPlayer::isVisible() const
	{
		if (*this == g_player)
		{
			return ENTITY::IS_ENTITY_VISIBLE(g_player_ped);
		}
		SOUP_IF_LIKELY (auto cmd = getCommand())
		{
			return cmd->syncing_as_visible;
		}
		return true;
	}

	void AbstractPlayer::giveOtr() const
	{
		uint64_t args[] = {
			Util::i32_to_u64_trickery(SE_OTR),
			g_player,
			0x7FFFFFFF,
			0x7FFFFFFF,
			1,
			1,
			(uint64_t)*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_SEVERIFY).as<int*>()
		};
		TSE(1 << p, args);

		if (auto cmd = getCommand())
		{
			cmd->given_otr_since = get_current_time_millis();
		}
	}

	void AbstractPlayer::openThirdEye(std::function<void(const AbstractPlayer&, AbstractEntity&, bool)>&& callback) const
	{
		if (!openThirdEyeWouldBeUseful())
		{
			AbstractEntity ped = getPed();
			callback(*this, ped, false);
			return;
		}

		const bool needs_cleanup = openThirdEyeBegin();
		const time_t deadline = get_current_time_millis() + 3000;
		Script::current()->yield();
		while (deadline > get_current_time_millis() && exists() && hasPed())
		{
			AbstractEntity ped = getPed();
			if (ped.isInBubble())
			{
				Script::current()->yield(); // give another tick for their vehicle to be loaded in
				callback(*this, ped, needs_cleanup);
				return;
			}
			openThirdEyeTick(needs_cleanup);
			Script::current()->yield();
		}
		openThirdEyeCleanup(needs_cleanup);
	}

	bool AbstractPlayer::openThirdEyeWouldBeUseful() const
	{
		return !getPed().isInBubble()
			&& PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(*this)
			;
	}

	bool AbstractPlayer::openThirdEyeBegin() const
	{
		auto cmd = getCommand()->getSpectateNinjaCommand();
		if (cmd->m_on)
		{
			return false;
		}
		Click click(CLICK_AUTO, TC_OTHER);
		cmd->setStateBool(click, true);
		return true;
	}

	void AbstractPlayer::openThirdEyeTick(bool needs_cleanup)
	{
		if (needs_cleanup)
		{
			g_renderer.drawCentredTextThisTick(LANG_GET_W("GEMINI"));
		}
	}

	void AbstractPlayer::openThirdEyeCleanup(bool needs_cleanup) const
	{
		if (needs_cleanup)
		{
			Click click(CLICK_AUTO, TC_OTHER);
			getCommand()->getSpectateNinjaCommand()->setStateBool(click, false);
		}
	}

	void AbstractPlayer::fiberTeleport(Vector2Plus coords) const
	{
		SOUP_ASSERT(ExecCtx::get().tc == TC_SCRIPT_YIELDABLE);

		if (coords.z_is_guessed)
		{
			for (int i = 0; i < 60; i++)
			{
				STREAMING::REQUEST_COLLISION_AT_COORD(coords.x, coords.y, coords.z);
				float ground_z = get_ground_z(coords.x, coords.y);
				if (ground_z != -200.0f)
				{
					coords.z = ground_z;
					break;
				}
				Script::current()->yield();
			}
		}

		return fiberTeleport((v3)coords);
	}

	void AbstractPlayer::fiberTeleport(const v3& pos) const
	{
		SOUP_ASSERT(ExecCtx::get().tc == TC_SCRIPT_YIELDABLE);

		if (getVehicle().isValid()) // Vehicle is in bubble?
		{
			time_t started_at = get_current_time_millis();
			while (exists()
				&& GET_MILLIS_SINCE(started_at) < 5000
				&& fiberTeleportVehicleTick(pos)
				)
			{
				Script::current()->yield();
			}
		}
		else if (openThirdEyeWouldBeUseful()) // Has vehicle but out of bubble?
		{
			time_t started_at = get_current_time_millis();
			const bool needs_cleanup = openThirdEyeBegin();

			while (exists()
				&& GET_MILLIS_SINCE(started_at) < 15000
				)
			{
				openThirdEyeTick(needs_cleanup);
				if (!fiberTeleportVehicleTick(pos))
				{
					break;
				}
				Script::current()->yield();
			}

			openThirdEyeCleanup(needs_cleanup);
		}
		else // On foot?
		{
			g_hooking.tp_target_id = getPed().getNetObjId();
			g_hooking.tp_destination = pos;
			using_model(ATSTRINGHASH("adder"), [this]
			{
				auto veh = Util::createVehicle(ATSTRINGHASH("adder"), getPos(), 0.0f);
				ENTITY::SET_ENTITY_VISIBLE(veh, false, false);
				ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(veh, false, false);
				ENTITY::FREEZE_ENTITY_POSITION(veh, true);

				g_hooking.tp_obj_id = veh.getNetObjId();
				for (int i = 0; i < 15; i++)
				{
					Script::current()->yield(50);
					veh.giveControl(*this);
				}

				veh.removeFromPlaneOfExistence();
				g_hooking.tp_obj_id = NETWORK_INVALID_OBJECT_ID;
			});
		}
	}

	bool AbstractPlayer::fiberTeleportVehicleTick(const v3& pos) const
	{
		if (auto veh = getVehicle(); veh.isValid())
		{
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(veh))
			{
				veh.setPos(pos);

				if constexpr (false)
				{
					if (auto clazz = VEHICLE::GET_VEHICLE_CLASS_FROM_NAME(veh.getModel());
						clazz == 8 // motorcycle
						|| clazz == 13 // cycle
						)
					{
						veh.setRot({ 180.0f, 0.0f, 0.0f });
					}
				}

				return false;
			}

			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(veh);
		}

		return true;
	}

	void AbstractPlayer::waypoint() const
	{
		Vector2Plus pos = getPos();
		if (!pos.isNull())
		{
			Util::setWaypoint(pos);
		}
	}

	soup::Optional<Vector2Plus> AbstractPlayer::getWaypoint() const
	{
		if (*this == g_player)
		{
			if (!g_gui.waypoint.isNull())
			{
				return g_gui.waypoint;
			}
		}
		else
		{
			if (auto cmd = getCommand())
			{
				if (cmd->has_waypoint)
				{
					return Vector2Plus(cmd->waypoint_x, cmd->waypoint_y);
				}
			}
		}
		return std::nullopt;
	}

	bool AbstractPlayer::canBeVoteKicked() const
	{
		return *ScriptGlobal(GLOBAL_VOTE_KICKED).at(p, 1).as<int*>() == 0
			&& canBeVoteKickedNaive()
			;
	}

	bool AbstractPlayer::canBeVoteKickedNaive() const
	{
		return !ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_SPECTATING).get<BOOL>()
			&& !(ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_BITSET1).get<int>() & (1 << GLOBAL_PLAYERREMOTE_BITSET1_NOVOTEKICK1))
			&& !(ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_BITSET1).get<int>() & (1 << GLOBAL_PLAYERREMOTE_BITSET1_NOVOTEKICK2))
			&& !(ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_FLAGS).get<int>() & (1 << GLOBAL_ORG_FLAGS_NOVOTEKICK3))
			&& !(ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_FLAGS).get<int>() & (1 << GLOBAL_ORG_FLAGS_NOVOTEKICK4))
			;
	}

	compactplayer_t AbstractPlayer::getNumKickVoters() const
	{
		compactplayer_t voters = 0;
		for (const auto& o : listExcept())
		{
			if (ScriptGlobal(GLOBAL_VOTE_KICK_DATA_BASE).at(o, GLOBAL_VOTE_KICK_DATA_SIZE).at(p, 1).get<int>() == 1)
			{
				++voters;
			}
		}
		return voters;
	}

	static constexpr compactplayer_t min_voters_for_kick = 2;

	compactplayer_t AbstractPlayer::getVoteKickThreshold()
	{
		auto threshold = getVoteKickThresholdUnclamped();
		return threshold < min_voters_for_kick ? min_voters_for_kick : threshold;
	}

	compactplayer_t AbstractPlayer::getVoteKickThresholdUnclamped()
	{
		return (compactplayer_t)floorf(float(NETWORK::NETWORK_GET_NUM_CONNECTED_PLAYERS() - 1) * ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_VOTE_KICKS_MULTIPLIER).get<float>());
	}

	bool AbstractPlayer::hasEnoughVotesToKick() const
	{
		const compactplayer_t voters = getNumKickVoters();
		return voters >= min_voters_for_kick && voters >= getVoteKickThresholdUnclamped();
	}

	Label AbstractPlayer::getHkReasonString(uint32_t reason)
	{
		switch (reason)
		{
		case VOTE_KICK: return LOC("PTX_HK_M_V");
		case BATTLEYE_KICK: return LOC("BE_K");
		case BATTLEYE_BAN: return LOC("BE_B");
		}
		return LOC("PTX_HK_M_N");
	}

	void AbstractPlayer::onPreRemove() const
	{
		if (*this != g_player)
		{
			const auto& gi = *getGamerInfo();
			for (const auto& entry : g_hooking.removed_players)
			{
				if (entry.second.appliesTo(gi))
				{
					return;
				}
			}
			g_hooking.removed_players.emplace_back(getName(), GamerIdentifierLatest::fromGamerInfo(gi));
		}
	}

	bool AbstractPlayer::kickFlowWait(time_t ms) const
	{
		const time_t deadline = get_current_time_millis() + ms;
		do
		{
			Script::current()->yield();
			if (!this->exists())
			{
				return true;
			}
			auto* command = getCommand();
			if (command != nullptr && command->has_left_script)
			{
				return true;
			}
		} while (!IS_DEADLINE_REACHED(deadline));
		return false;
	}

	void AbstractPlayer::kickBail() const
	{
		uint64_t args[] = {
			Util::i32_to_u64_trickery(SE_NETWORK_BAIL),
			g_player,
			Util::i32_to_u64_trickery(*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_SEVERIFY).as<int*>())
		};
		TSE(1 << p, args);
	}

	void AbstractPlayer::kick(bool use_pre_remove, bool can_take_script_host) const
	{
		if (!canUseUnblockableKick())
		{
			return;
		}

		if (use_pre_remove)
		{
			onPreRemove();
		}

		// If we're host, gg
		if (g_player.isHost())
		{
			kickHost(isKnownAdvertiser() ? BATTLEYE_BAN : DESYNC);
			return;
		}

		// Short-circuit for self
		if (p == g_player)
		{
			kickBail();
			return;
		}

		if (!isHost())
		{
			// HostShare
			if (AbstractPlayer::getHost().isStandUser())
			{
				AbstractPlayer::getHost().directPacketSend(DP_HOSTSHARE_KICK_DESYNC, *this);
			}

			// Love Letter Kick
			kickLoveLetter(true);
			if (kickFlowWait(10000))
			{
				return;
			}
		}

		// Now for kicks that let the target know, so put them in timeout.
		putInTimeout(TC_SCRIPT_YIELDABLE);

		// Orgasm kick
		if (!isHost()
			&& !isOrgasmKickOnCooldown()
			)
		{
			kickOrgasm();

			// See if that worked...
			if (kickFlowWait())
			{
				return;
			}
		}

		// Non-Host Kick
		kickNonHost();

		if (can_take_script_host)
		{
			// See if that worked...
			if (kickFlowWait())
			{
				return;
			}

			// Script Host Kick
			kickScriptHost();
		}
	}

	void AbstractPlayer::kickHost(eHostKickType type) const
	{
		onPreRemove();
		kickHostNoprekick(type);
	}

	void AbstractPlayer::kickHostNoprekick(eHostKickType type) const
	{
		if (auto info = getGamerInfoNoFallback())
		{
			rage::rlGamerHandle complainer{};
			pointers::CNetworkSession_KickPlayer(*pointers::network_session, info->getHandle(), 0, type, 0, complainer);
		}
	}

	void AbstractPlayer::kickScriptHost() const
	{
		if (canBeVoteKicked()
			&& ScriptHostUtil::canSafelyTakeIt()
			&& ScriptHostUtil::takeItYielding()
			)
		{
			*ScriptGlobal(GLOBAL_VOTE_KICKED).at(p, 1).as<int*>() = 1;
		}
	}

	// Will cause recipient to time out. Can be used as non-host.
	void AbstractPlayer::kickBeUpdate() const
	{
		unsigned char message[] = { 0x00, 0x50, 0x31, 0x4A, 0xC0, 0x1A, 0x13, 0xFF, 0xFF, 0xFF };
		for (int i = 0; i != 20; ++i)
		{
			message[0] = i;
			sendBePacket(false, message, sizeof(message));
		}
	}

	void AbstractPlayer::sendBePacket(bool client, const void* data, uint32_t size) const
	{
		SOUP_ASSERT(size < (1 << 11));

		char header_buf[32];
		rage::datBitBuffer header;
		header.SetReadWriteBytes(header_buf, sizeof(header_buf));
		header.writeU32(size, 11);
		header.writeBool(client);

		char content_buf[1 << 11];
		rage::datBitBuffer content;
		content.SetReadWriteBytes(content_buf, sizeof(content_buf));
		rage::netMessage::WriteHeader(143, content);
		content.WriteBytes(header_buf, header.GetByteLength());
		content.WriteBytes(data, size);

		if (rage::snSession* const session = (*pointers::network_session)->session)
		{
			(*pointers::connection_mgr)->Send(getCxnId(session->channel_id), content_buf, content.GetByteLength(), 0x4000001, nullptr);
		}
	}

	// Assuming this is only called on remote players. Otherwise only kickBail should be used.
	void AbstractPlayer::kickNonHost() const
	{
		kickBail();

		if (soup::rand.coinflip())
		{
			kickBeUpdate();
		}

		if (!isInAnOrg()
			&& soup::rand.coinflip()
			)
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_28),
				g_player,
				soup::rand(32, 0x7fffffff)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_INT_INVITE),
				g_player,
				soup::rand(163, 0x7fffffff),
				0,
				0,
			};
			TSE(1 << p, args);
		}
		
		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_8),
				g_player,
				0,
				soup::rand(26, 0x7fffffff),
			};
			TSE(1 << p, args);
		}
		
		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_26),
				g_player,
				soup::rand(0, 0x7fffffff),
				soup::rand(0, 0x7fffffff),
				soup::rand(0, 0x7fffffff),
				0,
				0,
				soup::rand(32, 0x7fffffff),
				0,
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_WARP_INTERIOR),
				g_player,
				soup::rand(162, 0x7fffffff),
				0,
				0,
				1,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				(uint64_t)-1
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			// the bounds check on this is >31, but should be >=20, lol
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_25),
				g_player,
				soup::rand(20, 0x7fffffff),
				0
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			// the bounds check on this is >31, but should be >=20, lol
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_10),
				g_player,
				soup::rand(20, 0x7fffffff),
				1
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_24),
				g_player,
				0,
				soup::rand(32, 0x7fffffff)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_20),
				g_player,
				soup::rand(7, 0x7fffffff),
				soup::rand(4, 0x7fffffff)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_18),
				g_player,
				0,
				0,
				soup::rand(220, 0x7fffffff)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_17),
				g_player,
				1,
				1,
				1,
				soup::rand(220, 0x7fffffff)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_15),
				g_player,
				0,
				0,
				0,
				soup::rand(32, 0x7fffffff),
				0,
				0
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_19),
				g_player,
				soup::rand(32, 0x7fffffff)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_12),
				g_player,
				1,
				soup::rand(124, 0x7fffffff)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_23),
				g_player,
				0,
				soup::rand(79, 0x7fffffff),
				0,
				0,
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_BOUNDS_KICK_6),
				g_player,
				0,
				soup::rand(26, 0x7fffffff)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_NOTIFY),
				g_player,
				Util::i32_to_u64_trickery(SE_NOTIFY_AMCH_POS),
				0, // score
				0, // place - 1
				0,
				0,
				0,
				0,
				0,
				Util::generate_invalid_player_id(), // 10: player that was beat
				0,
				0,
				0
			};
			TSE(1 << p, args);
		}

		// "Zero kicks" exploit the fact that GTA expects structs as script event parameters, and script structs are size-prefixed,
		// so, if we send no parameters, they default to zero, and the script will crash trying to access values which are out of bounds on this zero-sized struct.
		{
			constexpr uint64_t hashes[] = {
				Util::i32_to_u64_trickery(SE_ZERO_KICK_10),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_9),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_8),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_7),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_6),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_5),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_4),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_3),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_2),
				Util::i32_to_u64_trickery(SE_ZERO_KICK_1),
				Util::i32_to_u64_trickery(SE_UNUSED_TELEPORT),
			};
			for (const auto& hash : hashes)
			{
				if (soup::rand.coinflip())
				{
					continue;
				}
				if (soup::rand.coinflip())
				{
					uint64_t args[] = {
						hash,
						g_player
					};
					TSE(1 << p, args);
				}
				else
				{
					uint64_t args[] = {
						hash,
						g_player,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
					};
					TSE(1 << p, args);
				}
			}
		}

		{
			constexpr uint64_t hashes[] = {
				Util::i32_to_u64_trickery(SE_STAT_READ_INT),
				Util::i32_to_u64_trickery(SE_STAT_READ_FLOAT),
				Util::i32_to_u64_trickery(SE_STAT_READ_BOOL),
			};
			for (const auto& hash : hashes)
			{
				if (soup::rand.coinflip())
				{
					continue;
				}
				uint64_t args[] = {
					hash,
					g_player,
					0,
					0x7fffffff // stat index
				};
				TSE(1 << p, args);
			}
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_UNK_KICK_3),
				g_player,
				0,
				0,
				0,
				1
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_UNK_KICK_2),
				g_player,
				28,
				Util::i32_to_u64_trickery(-1),
				Util::i32_to_u64_trickery(-1)
			};
			TSE(1 << p, args);
		}

		if (soup::rand.coinflip())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_UNK_KICK_1),
				g_player,
				soup::rand.t<uint64_t>(50, 0x7FFFFFFF),
				0
			};
			TSE(1 << p, args);
		}

		if (isScriptHost()
			&& soup::rand.coinflip()
			)
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_ZERO_KICK_11),
				g_player
			};
		}
	}

	bool AbstractPlayer::checkAggressiveAction() const
	{
#if BAD_BOY_ENABLED
		if (auto cmd = getCommand())
		{
			if (cmd->classifier.hasNonPreemptiveDetectionsThatAllowAggressiveReaction()
				|| cmd->aggressive_action_warranted
				)
			{
				return true;
			}
			if (cmd->hasStandUserHandshakeFinished())
			{
				if (cmd->isMarkedAsStandUser() // Marked as a Stand user?
					&& !(cmd->dp_flags & DPFLAG_COLOADING) // not co-loading?
#ifdef STAND_DEBUG
					&& (cmd->dp_flags & DPFLAG_SUION) // not "S.U.I. Off"?
#endif
					&& GET_MILLIS_SINCE(cmd->discovery) <= 15000 // target only joined recently?
					)
				{
					g_gui.putInBadBoyTimeout();
					return false;
				}
				return true;
			}
		}
		return false;
#else
		return true;
#endif
	}

	bool AbstractPlayer::canUseUnblockableKick(bool karma_ourselves) const
	{
#if ASSUME_NO_RID_SPOOFING
		if (is_stand_dev_rid(getRockstarId()))
		{
			if (karma_ourselves
				&& ColoadMgr::coloading_with_any_menu // Only karma if we're coloading, since we want to prevent the coloaded menu followup kick once Stand refuses.
				)
			{
				TransitionHelper::beAlone();
			}
			return false;
		}
		return true;
#else
		CommandPlayer* const command = getCommand();
		return command != nullptr
			&& command->hasStandUserHandshakeFinished()
			&& !(command->dp_flags & DPFLAG_DEVDBG)
			;
#endif
	}

	void AbstractPlayer::kickLoveLetter(bool call_pre_remove) const
	{
		if (performing_love_letter_kick || !canUseUnblockableKick())
		{
			return;
		}
		if (call_pre_remove)
		{
			onPreRemove();
		}
		performing_love_letter_kick = true;
		FiberPool::queueJob([target{ *this }]
		{
			time_t start = get_current_time_millis();
			while (true)
			{
				if (!target.exists() || GET_MILLIS_SINCE(start) > 10000)
				{
					break;
				}

				rage::netComplaintMsg packet;
				packet.sender = g_player.getPeerId();
				packet.targets[0] = target.getPeerId();
				packet.num_targets = 1;
				pointers::send_netComplaintMsg(*pointers::connection_mgr, AbstractPlayer::getHost().getEndpoint()->GetAddress(), (*pointers::network_session)->session->channel_id, &packet, 0);

				Script::current()->yield();
				Script::current()->yield();
			}
			performing_love_letter_kick = false;
		});
	}

	bool AbstractPlayer::isOrgasmKickOnCooldown()
	{
		return get_current_time_millis() <= next_orgasm_kick;
	}

	void AbstractPlayer::kickOrgasm() const
	{
		next_orgasm_kick = get_current_time_millis() + 10'000;

		if (!is_session_transition_active(true))
		{
			if (auto player = getCNetGamePlayer())
			{
				if (auto thread = GtaThread::fromHash(ATSTRINGHASH("freemode")))
				{
					if (auto netcomp = thread->getNetComponent())
					{
						onPreRemove();

						rage::msgScriptMigrateHost msg{};
						msg.script_id = &thread->m_handler->GetScriptId();
						msg.host_token = netcomp->host_token;
						msg.net_array_data_size = g_player.getCNetGamePlayer()->m_sizeOfNetArrayData;
						for (int i = 0; i != 1097; ++i)
						{
							rage::scriptHandlerMgr::SendMessageToPlayer(*player, msg);
						}
						return;
					}
				}
			}
		}

		if (rage::snSession* const session = (*pointers::network_session)->session)
		{
			MsgRadioStationSyncRequest msg;
			for (int i = 0; i != 1097; ++i)
			{
				(*pointers::connection_mgr)->Send(getCxnId(session->channel_id), msg, 1u);
			}
		}
	}

	void AbstractPlayer::kickBreakup(int remove_reason) const
	{
		if (canUseUnblockableKick())
		{
			LeaveReasons::getEntry(getPeerId()).breakup_from = g_player;
#if HAVE_BREAKUP_KICK
			if (g_player.isHost())
#endif
			{
				sessionDesync(getGamerHandle(), remove_reason);
				desyncBreakup(remove_reason);
			}
#if HAVE_BREAKUP_KICK
			else if (isHost())
			{
				sessionDesync(remove_reason);
				desyncBreakup(remove_reason);
			}
			else
			{
				getHost().remoteDesync(getGamerHandle(), remove_reason);
			}
#endif
		}
		else
		{
#if ASSUME_NO_RID_SPOOFING
			// Let Stand admins know if breakup/ban kick was used on them. Assuming that canUseUnblockableKick is only false for us.
			kickBail();
#endif
		}
	}

#if HAVE_BREAKUP_KICK
	void AbstractPlayer::kickConfusion() const
	{
		remoteDesync(getHost().getGamerHandle());
	}
#endif

	void AbstractPlayer::sessionDesync(int remove_reason) const
	{
		sessionDesync(getGamerHandle(), remove_reason);
	}

	void AbstractPlayer::sessionDesync(const rage::rlGamerHandle& gamer_handle, int remove_reason) const
	{
		for (const auto& p : AbstractPlayer::listExcept(true))
		{
			if (p != *this)
			{
				p.remoteDesync(gamer_handle, remove_reason);
			}
		}
	}

	void AbstractPlayer::remoteDesync(const rage::rlGamerHandle& target, int remove_reason) const
	{
		if (auto endpoint = getEndpoint())
		{
			rage::snSession* const session = (*pointers::network_session)->session;
			endpoint->removeGamer(
				session->channel_id,
				session->GetSessionId(),
				target,
				remove_reason
			);
		}
	}

	void AbstractPlayer::desyncBealone() const
	{
		if (g_player.isHost())
		{
			removeFromPlayerMgr();
		}
		else
		{
			removeFromSessionMgr();
		}
	}

	void AbstractPlayer::desyncBreakup(int remove_reason) const
	{
		if (isHost()
			|| remove_reason != -1
			)
		{
			removeFromSessionMgr(remove_reason);
		}
		else
		{
			removeFromPlayerMgr();
		}
	}

	void AbstractPlayer::removeFromPlayerMgr() const
	{
		if (auto cngp = getCNetGamePlayer())
		{
			(*pointers::network_player_mgr)->removePlayer(cngp);
		}
	}

	void AbstractPlayer::removeFromSessionMgr(int remove_reason) const
	{
		rage::snSession* const session = (*pointers::network_session)->session;
		const auto con_id = g_player.isHost()
			? getCxnId(session->channel_id)
			: AbstractPlayer::getHost().getCxnId(session->channel_id)
			;
		if (auto peer = session->FindPeerByCxn(con_id))
		{
			rage::snMsgRemoveGamersFromSessionCmd msg;
			msg.session_id = session->GetSessionId();
			msg.targets[0] = getGamerHandle();
			msg.remove_reason = remove_reason;
			msg.num_targets = 1;
			pointers::rage_snSession_HandleRemoveGamerCmd(
				session,
				peer,
				&msg
			);
		}
	}

	void AbstractPlayer::kickPickup() const
	{
		onPreRemove();

		using_model(ATSTRINGHASH("vw_prop_vw_colle_prbubble"), [this]
		{
			auto* const player_cmd = getCommand();
			if (player_cmd == nullptr)
			{
				return;
			}
			AbstractEntity ped = getPed();
			if (!ped.isValid())
			{
				return;
			}
			auto* const freezeCommand = ((CommandToggleNoCorrelation*)player_cmd->recursivelyResolveChildByMenuName(LOC("FRZ")));
			const bool freeze_prev_on = freezeCommand->m_on;
			Click click(CLICK_AUTO, TC_SCRIPT_YIELDABLE);
			freezeCommand->setStateBool(click, true);
			openThirdEye([](const AbstractPlayer& p, AbstractEntity& ped, bool needs_cleanup)
			{
				Script::current()->yield(1000);
				for (int i = 0; i < 5; i++)
				{
					const v3 player_pos = ped.getPos();
					OBJECT::CREATE_AMBIENT_PICKUP(ATSTRINGHASH("PICKUP_PORTABLE_CRATE_UNFIXED_INCAR_SMALL"), player_pos.x, player_pos.y, player_pos.z, 0b10001, 100, ATSTRINGHASH("vw_prop_vw_colle_prbubble"), 0, 0);
					Script::current()->yield(1000);
				}
				if (needs_cleanup)
				{
					p.openThirdEyeCleanup(needs_cleanup);
				}
			});
			freezeCommand->setStateBool(click, freeze_prev_on);
		});
	}

	bool AbstractPlayer::canSendNetEvents(int count)
	{
		return *pointers::rage_netGameEvent_ms_pPool
			&& ((*pointers::rage_netGameEvent_ms_pPool)->GetNoOfFreeSpaces() - 50) >= count
			;
	}

	void AbstractPlayer::crash() const
	{
		if (isRockstarAdmin())
		{
			return;
		}

		onPreRemove();
		putInTimeout(TC_SCRIPT_YIELDABLE);

		// Kindly ask them to delete their own player ped
		sendCloneDelete(getPed().getNetObjId());

#if false
		if (!isScriptHost())
		{
			if (ScriptHostUtil::canSafelyTakeIt())
			{
				if (auto fm = GtaThread::fromHash(ATSTRINGHASH("freemode")))
				{
					if (fm->m_net_component)
					{
						fm->m_net_component->giveHost(getCNetGamePlayer());
						Script::current()->yield();
					}
				}
			}
		}
#endif

#if true
		if (isScriptHost())
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_SH_SPAWN_VEH),
				g_player,
				soup::rand.t<uint64_t>(0xFFFF'0000, 0xFFFF'FFFF),
				0,
				0,
				0,
				soup::rand.t<uint64_t>(0xFFFF'0000, 0xFFFF'FFFF),
				soup::rand.t<uint64_t>(0xFFFF'0000, 0xFFFF'FFFF)
			};
			TSE(1 << p, args);
		}
#endif
	}

	void AbstractPlayer::sendCloneDelete(uint16_t netobjid) const
	{
		char pack_buf[BIT_BUFFER_SIZE(18)];
		rage::datBitBuffer pack_bb;
		pack_bb.SetReadWriteBytes(pack_buf, sizeof(pack_buf));
		pack_bb.writeU32(netobjid, 13); // object id
		pack_bb.writeU32(0, 5); // ownership token

		char buf[912];
		rage::datBitBuffer bb;
		bb.SetReadWriteBytes(buf, sizeof(buf));
		rage::netMessage::WriteHeader(PACKED_RELIABLES_MSG, bb);
		bb.writeU32(4, 4); // have removes
		bb.writeU32(1, 5); // have message
		bb.writeU32(18, 13); // 18 bits
		bb.WriteBits(pack_buf, 18);

		(*pointers::network_player_mgr)->SendBuffer(getCNetGamePlayer(), buf, bb.GetByteLength(), 1u, nullptr, nullptr);
	}

	bool AbstractPlayer::isInScope(playerscope_t scope) const
	{
		switch (scope)
		{
		case PlayerScope::EVERYONE:
			return true;

		case PlayerScope::CREW_MEMBERS:
			if (isCrewMember())
			{
				return true;
			}
			[[fallthrough]];
		case PlayerScope::FRIENDS:
			if (isFriend())
			{
				return true;
			}
			[[fallthrough]];
		case PlayerScope::SELF:
			return (*this == g_player);
		}
		return false;
	}

	static size_t sent_messages = 0;
	static size_t sent_ads = 0;
	static std::unordered_set<std::string> advertisements_sent{};
	static std::unordered_set<uint64_t> people_advertised_to{};
	static size_t ad_recipients_threshold = 150;
	static time_t last_suspend_request = 0;

	void AbstractPlayer::onComm(const std::string& _text, rage::netPeerId recipient_startup_id)
	{
		auto text = StringUtils::utf8_to_utf16(_text);
		auto ad_level = g_player.getMessageAdLevel(StringUtils::utf8_to_utf16(_text));
		normaliseForAdFilters(text);
		if (!messageIsEmptyOrMiscPromotion(text))
		{
			++sent_messages;
		}
		if (ad_level != 0)
		{
			++sent_ads;
		}
		if (ad_level > 1)
		{
#ifdef STAND_DEV
			Util::toast(fmt::format(fmt::runtime(soup::ObfusString("Your outgoing message was detected as advertising (level {}). You're using a dev build so I hope you're not advertising! Please report this false-positive!").str()), ad_level));
#endif
			advertisements_sent.emplace(_text);
			people_advertised_to.emplace(recipient_startup_id);
			if (ad_level == 4)
			{
				ad_recipients_threshold = 70;
			}
			float advertising_percentage = (float)sent_ads / (float)sent_messages;
			if (people_advertised_to.size() >= ad_recipients_threshold // enough people have received an advertisement?
				&& (advertising_percentage > 0.4) // and more than 40% of all outgoing messages are advertisements?
				)
			{
				if (g_auth.hasApiCredentials())
				{
					if (GET_MILLIS_SINCE(last_suspend_request) > 3000)
					{
						std::string data = g_player.getSessionName();
						for (const auto& msg : advertisements_sent)
						{
							data.append(soup::ObfusString(" % ").str());
							data.append(msg);
						}
						g_auth.reportEvent("SA", std::move(data));
						last_suspend_request = get_current_time_millis();
					}
				}
				else
				{
					yeet_now();
				}
			}
		}
	}

	bool AbstractPlayer::isKnownAdvertiser() const
	{
		if (auto gi = getGamerInfoNoFallback())
		{
			return gi->isKnownAdvertiser();
		}
		return false;
	}

	uint8_t AbstractPlayer::getMessageAdLevel(const std::string& text) const
	{
		return getMessageAdLevel(StringUtils::utf8_to_utf16(text));
	}

	uint8_t AbstractPlayer::getMessageAdLevel(std::wstring text) const
	{
		normaliseForAdFilters(text);

		auto name = StringUtils::utf8_to_utf16(getSessionName());
		normaliseForAdFilters(name);

		// Check if message has call to action & promise of good things
		bool name_has_call_to_action = nameIsCallToAction(name);
		bool msg_has_call_to_action = messageHasCallToAction(text);
		bool has_promise_of_good_things = messageHasPromiseOfGoodThings(text);
		if ((name_has_call_to_action || msg_has_call_to_action) && has_promise_of_good_things)
		{
			if (auto cmd = getCommand())
			{
				cmd->last_message_had_call_to_action = true;
				cmd->last_message_had_promise_of_good_things = true;
			}
			return 4;
		}

		// Blacklist on message
		if (checkKnownAggressiveSpammer(text))
		{
			return 3;
		}

		// Blacklist on name
		if (checkKnownAggressiveSpammer(name))
		{
			return 3;
		}

		// Handle call to action & promise of good things being split into separate messages
		if ((has_promise_of_good_things || msg_has_call_to_action) && isKnownAdvertiser())
		{
			return 2;
		}
		else if (auto cmd = getCommand())
		{
			if (!msg_has_call_to_action && !has_promise_of_good_things)
			{
				if (!messageIsEmptyOrMiscPromotion(text))
				{
					cmd->last_message_had_call_to_action = false;
					cmd->last_message_had_promise_of_good_things = false;
				}
			}
			else
			{
				uint8_t level = 0;
				if (GET_MILLIS_SINCE(cmd->last_message_time) < 20'000
					&& ((has_promise_of_good_things && cmd->last_message_had_call_to_action)
						|| (msg_has_call_to_action && cmd->last_message_had_promise_of_good_things)
						)
					)
				{
					level = 2;
				}
				else if (StringUtils::isBasicallyTheSame(text, cmd->last_message))
				{
					level = 1;
				}
				cmd->last_message = std::move(text);
				cmd->last_message_time = get_current_time_millis();
				cmd->last_message_had_call_to_action = msg_has_call_to_action;
				cmd->last_message_had_promise_of_good_things = has_promise_of_good_things;
				return level;
			}
		}

		// All good
		return 0;
	}

	void AbstractPlayer::normaliseForAdFilters(std::wstring& str) noexcept
	{
		// Latinise homoglyphs
		for (auto& c : str)
		{
			Sainan::transform_homoglyph(c);
		}

		// Ensure lowercase
		StringUtils::to_lower(str);
	}

	bool AbstractPlayer::checkKnownAggressiveSpammer(const std::wstring& msg) noexcept
	{
		if (BgScript::hasFunction("ka"))
		{
			return BgScript::query("ka", StringUtils::utf16_to_utf8(msg));
		}

		return msg.find(L"gtavkrutka") != std::wstring::npos
			|| msg.find(L"p520.cc") != std::wstring::npos
			|| msg.find(L"p620.cc") != std::wstring::npos
			|| msg.find(L"p777.me") != std::wstring::npos
			|| msg.find(L"p520.me") != std::wstring::npos
			|| msg.find(L"p888.me") != std::wstring::npos
			|| msg.find(L"shopgta5.com") != std::wstring::npos
			|| msg.find(L"sentinelgames.store") != std::wstring::npos
			|| msg.find(L"莱斯特电玩") != std::wstring::npos // Selling Recovery on WeChat
			|| msg.find(L"莱信斯公特") != std::wstring::npos // Selling Recovery on WeChat
			;
	}

	bool AbstractPlayer::nameIsCallToAction(const std::wstring& name)
	{
		return BgScript::query("cl", StringUtils::utf16_to_utf8(name));
	}

	bool AbstractPlayer::messageHasCallToAction(const std::wstring& msg)
	{
		return BgScript::query("c", StringUtils::utf16_to_utf8(msg));
	}

	bool AbstractPlayer::messageHasPromiseOfGoodThings(const std::wstring& msg)
	{
		return BgScript::query("p", StringUtils::utf16_to_utf8(msg));
	}

	bool AbstractPlayer::messageIsEmptyOrMiscPromotion(const std::wstring& msg)
	{
		if (StringUtils::simplify(msg).empty())
		{
			return false;
		}

		return BgScript::query("mp", StringUtils::utf16_to_utf8(msg));
	}

	void AbstractPlayer::sendSms(const std::string& text) const
	{
		// Empty text seems to break ability to receive further messages
		if (text.empty())
		{
			return;
		}

		if (auto peer = (*pointers::network_session)->session->FindPeerById(getPeerId()))
		{
			onComm(text, getStartupId());

			MsgTextMessage packet{};
			packet.setText(text.c_str());
			packet.sender = g_player.getRockstarId();

			pointers::send_MsgTextMessage(*pointers::connection_mgr, peer->cxn_id, &packet, 1, nullptr);
		}
	}

	void AbstractPlayer::directPacketSend(DirectPacket a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6) const
	{
		uint64_t flags = 0;
		if (ColoadMgr::coloading_with_any_menu)
		{
			flags |= DPFLAG_COLOADING;
		}
		if (g_auth.isSuiEnabledThisSession())
		{
			flags |= DPFLAG_SUION;
		}
#ifdef STAND_DEBUG
		flags |= DPFLAG_DEVDBG;
		if (g_gui.meta_state.data.contains("Developer"))
		{
			flags |= DPFLAG_DETASDEV; // If we want to show up as "Stand User (Developer)"
		}
#endif

		uint64_t args[] = {
			g_auth.direct_packet_magic, // 0: Magic Number
			g_player, // 1: Sender
			flags, // 2: Flags
			a0, // 3+: Arbitrary Data
			a1,
			a2,
			a3,
			a4,
			a5,
			a6,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		if (a1 == 0
			&& a2 == 0
			&& a3 == 0
			&& a4 == 0
			&& a5 == 0
			&& a6 == 0
			)
		{
			tse_impl(args, soup::rand.t<int>(4, 9), 1 << p);
		}
		else
		{
			tse_impl(args, soup::rand.t<int>(11, 20), 1 << p);
		}
	}

	void AbstractPlayer::directPacketSendIdentify(DirectPacket a0) const
	{
		return directPacketSend(a0);
	}

	uint8_t AbstractPlayer::getAuthInfo() const
	{
		for (const auto& gi : g_auth.remote_session_peers)
		{
			if (gi->appliesTo(*this))
			{
				if (gi->is_on_devdbg)
				{
					return 2;
				}
				return 1;
			}
		}
		return 0;
	}

	v3 AbstractPlayer::getCamPos() const
	{
		if (*this == g_player)
		{
			return CAM::GET_FINAL_RENDERED_CAM_COORD();
		}
		if (auto n = getPed().getNetObject<CNetObjPlayer>())
		{
			return n->viewport->m_Camera44.getTranslate();
		}
		return {};
	}

	v3 AbstractPlayer::getCamRot() const
	{
		if (*this == g_player)
		{
			return CAM::GET_FINAL_RENDERED_CAM_ROT(2);
		}
		v3 rot{};
		if (auto n = getPed().getNetObject<CNetObjPlayer>())
		{
			rot = n->viewport->m_Camera44.getRotationXYZ();
			// y = 0
			rot.x -= 90.0f;
		}
		return rot;
	}

	AbstractPlayer AbstractPlayer::getSpectateTarget() const
	{
		if (auto cmd = getCommand())
		{
			return cmd->spectating;
		}
		return invalid();
	}

	bool AbstractPlayer::isThisPositionInCameraOrbit(const v3& pos) const
	{
		// Using topdown distance because if spectator is in facility, the camera position is synched much further below what it should be.
		return getPos().distanceTopdown(pos) <= (isInVehicle() ? 50.0f : 10.0f);
	}

	float AbstractPlayer::getWeaponDamageModifier() const
	{
		if (auto ped = getCPedPtr())
		{
			if (auto pi = ped->player_info)
			{
				return pi->m_fWeaponDamageModifier;
			}
		}
		return 0.71358746f;
	}

	float AbstractPlayer::getMeleeWeaponDamageModifier() const
	{
		if (auto ped = getCPedPtr())
		{
			if (auto pi = ped->player_info)
			{
				return pi->m_fMeleeWeaponDamageModifier;
			}
		}
		return 1.0f;
	}

	void AbstractPlayer::giveVehicleRepair() const
	{
		if (*this == g_player)
		{
			g_player_veh.repair();
		}
		else
		{
			givePickupReward(ATSTRINGHASH("REWARD_VEHICLE_FIX"));
		}
	}

	void AbstractPlayer::giveBounty(int value) const
	{
		uint64_t args[] = {
			Util::i32_to_u64_trickery(SE_BOUNTY),
			g_player, // 1: Sender
			p, // 2: Target
			1,
			(uint64_t)value, // 4: Value
			0,
			1, // 6: From NPC
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			(uint64_t)*ScriptGlobal(GLOBAL_SEVERIFY).at(9).as<int*>(),
			(uint64_t)*ScriptGlobal(GLOBAL_SEVERIFY).at(10).as<int*>(),
		};

		TSE(Util::get_session_players_bitflag(), args);
	}
}
