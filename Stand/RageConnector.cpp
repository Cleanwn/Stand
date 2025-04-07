#include "RageConnector.hpp"

#include "AbstractPlayer.hpp"
#include "AddressGamers.hpp"
#include "FiberPool.hpp"
#include "nettypes.hpp"
#include "netConnection.hpp"
#include "netConnectionManager.hpp"
#include "netEndpoint.hpp"
#include "netStatus.hpp"
#include "NetworkSessionMessages.hpp"
#include "pointers.hpp"
#include "rl.hpp"
#include "rlGamerInfo.hpp"
#include "rlSessionDetail.hpp"
#include "rlSessionInfo.hpp"
#include "Script.hpp"
#include "snet_messages.hpp"

#define RC_DEBUG false

#if RC_DEBUG
static_assert(STAND_DEBUG);
#include "ScAccount.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"
#endif

namespace Stand
{
	static std::unordered_set<uint64_t> pending_connections{};
	static std::unordered_map<uint64_t, rage::rlSessionDetail> cached_details{};

	bool RageConnector::isRequestActive()
	{
		return !pending_connections.empty();
	}

	size_t RageConnector::getNumActiveRequests()
	{
		return pending_connections.size();
	}

	bool RageConnector::hasConnection(const rage::netPeerId startup_id)
	{
		if (auto endpoint = (*pointers::connection_mgr)->getEndpointByStartupId(startup_id))
		{
			return endpoint->peer_address.startup_id == startup_id;
		}
		return false;
	}

	rage::netEndpoint* RageConnector::getConnection(const rage::netPeerId startup_id)
	{
		return (*pointers::connection_mgr)->getEndpointByStartupId(startup_id);
	}

	uint32_t RageConnector::getChannelId(const rage::netPeerId startup_id)
	{
		uint32_t channelId = NETWORK_SESSION_GAME_CHANNEL_ID;
		if (auto detail = getCachedSessionDetail(startup_id))
		{
			if (detail->m_SessionConfig.m_Attrs.m_SessionPurpose != 0)
			{
				channelId = NETWORK_SESSION_ACTIVITY_CHANNEL_ID;
			}
		}
		return channelId;
	}

	bool RageConnector::isConnectingTo(const rage::netPeerId startup_id)
	{
		return pending_connections.contains(startup_id);
	}

	void RageConnector::connect(const rage::rlSessionInfo& si)
	{
		if (hasConnection(si.host.startup_id)
			|| isConnectingTo(si.host.startup_id)
			)
		{
			return;
		}
		pending_connections.emplace(si.host.startup_id);
		FiberPool::queueJob([si]
		{
#if RC_DEBUG
			Util::toast(fmt::format("Connecting to {}...", ScAccount::fromRID(si.host.gamer_handle.rockstar_id)->toString(false)));
#endif

			// This stuff needs to stick around for the duration of the task, storing it on the fiber's stack is fine.
			rage::rlSessionDetail details;
			unsigned int numDetails;
			rage::netStatus status;

			pointers::rage_rlSessionManager_QueryDetail(
				RL_NETMODE_ONLINE,
				NETWORK_SESSION_GAME_CHANNEL_ID,
				0,
				/* HOST_QUERY_MAX_ATTEMPTS */ 6,
				1,
				true,
				3,
				{},
				&si,
				1,
				&details,
				&numDetails,
				&status
			);

			while (status.isPending())
			{
				Script::current()->yield();
			}

			pending_connections.erase(si.host.startup_id);

			if (auto endpoint = (*pointers::connection_mgr)->getEndpointByStartupId(si.host.startup_id);
				endpoint && endpoint->peer_address.startup_id == si.host.startup_id
				)
			{
				//if (status.code == rage::netStatus::SUCCESS)
				if (details.m_HostName[0] != '\0')
				{
#if RC_DEBUG
					Util::toast(fmt::format("Connected to {}, who says their name is {}.", endpoint->address.sock_addr.ip.toString<std::string>(), (const char*)details.m_HostName));
#endif

					if (auto e = cached_details.find(si.host.startup_id); e != cached_details.end())
					{
						e->second = std::move(details);
					}
					else
					{
						cached_details.emplace(si.host.startup_id, std::move(details));
					}
					AddressGamers::onDiscoveredName(endpoint->getRemoteAddress().ip, details.m_HostName);
					return;
				}
				else
				{
#if RC_DEBUG
					Util::toast(fmt::format("Connected to {}, who provided no details.", endpoint->getRemoteAddress().ip.toString<std::string>()));
#endif
				}
			}
			else
			{
#if RC_DEBUG
				Util::toast("Connection failed.");
#endif
			}
			if (auto e = cached_details.find(si.host.startup_id); e != cached_details.end())
			{
				cached_details.erase(e);
			}
		});
	}

#if I_CAN_SPY
	void RageConnector::destroyCxn(rage::Cxn* cxn)
	{
		cxn->endpoint->channel_cxns[cxn->channel_index] = nullptr;
		pointers::rage_netConnectionManager_DestroyConnection(*pointers::connection_mgr, cxn);
	}
#endif

	rage::rlSessionDetail* RageConnector::getCachedSessionDetail(const rage::netPeerId startup_id)
	{
		if (auto e = cached_details.find(startup_id); e != cached_details.end())
		{
			return &e->second;
		}
		return nullptr;
	}

#if I_CAN_SPY
	static std::vector<Spy> spies{};
#endif

	bool RageConnector::isSpyActive()
	{
#if I_CAN_SPY
		return !spies.empty();
#else
		return false;
#endif
	}

#if I_CAN_SPY
	Spy* RageConnector::spyFindCxnId(int32_t cxnId)
	{
		for (auto& spy : spies)
		{
			if (spy.cxn->id == cxnId)
			{
				return &spy;
			}
		}
		return nullptr;
	}

	Spy* RageConnector::spyFindSessionId(uint64_t session_id)
	{
		for (auto& spy : spies)
		{
			if (spy.session_id == session_id)
			{
				return &spy;
			}
		}
		return nullptr;
	}

	void RageConnector::sendJoinRequest(const rage::netPeerId host_startup_id)
	{
		if (auto detail = getCachedSessionDetail(host_startup_id))
		{
			auto endpoint = getConnection(host_startup_id);

			Spy& spy = spies.emplace_back(Spy{});
			spy.cxn = pointers::rage_netConnectionManager_CreateConnection(
				*pointers::connection_mgr,
				endpoint,
				detail->m_SessionConfig.m_Attrs.m_SessionPurpose == 0 ? NETWORK_SESSION_GAME_CHANNEL_ID : NETWORK_SESSION_ACTIVITY_CHANNEL_ID
			);
			spy.session_id = detail->m_SessionInfo.id;

			char data[SNET_MAX_SIZEOF_GAMER_DATA];
			unsigned int sizeofdata;
			pointers::CNetworkSession_BuildJoinRequest(*pointers::network_session, data, sizeof(data), &sizeofdata, 3);

#if false
			using rage_snMsgJoinRequest_Reset_t = void(__fastcall*)(void*, uint64_t sessionId, rage::rlGamerInfo*, const rage::netAddress* relayAddr, int slotType, void* data, size_t sizeofData, const rage::rlGamerHandle* groupMembers, int groupSize);
			auto rage_snMsgJoinRequest_Reset = (rage_snMsgJoinRequest_Reset_t)IDA_ADDR(00007FF72554774C);

			char jr[0x1000];
			rage_snMsgJoinRequest_Reset(jr, detail->m_SessionInfo.id, g_player.getGamerInfo(), rage_netRelay_GetMyRelayAddress(), 1, data, sizeofdata, &g_player.getGamerHandle(), 1);
#else
			rage::snMsgJoinRequest jr;
			jr.session_id = detail->m_SessionInfo.id;
			jr.m_GamerInfo = *g_player.getGamerInfo();
			jr.m_RelayAddr = *pointers::rage_netRelay_GetMyRelayAddress();
			//jr.m_RelayAddr = rage::netAddress(jr.m_GamerInfo.peer.address.relay, jr.m_GamerInfo.peer.address.online, jr.m_GamerInfo.peer.address.relay_token);
			jr.m_SlotType = rage::RL_SLOT_PRIVATE;
			jr.m_SizeofData = sizeofdata;
			memcpy(jr.m_Data, data, sizeofdata);
			jr.m_GroupSize = 1;
			jr.m_GroupMembers[0] = jr.m_GamerInfo.getHandle();
#endif

			char buf[0x1000];
			unsigned int sizeofbuf;
#if true
			pointers::rage_snMsgJoinRequest_Export(&jr, buf, sizeof(buf), &sizeofbuf);
#else
			jr.Export<rage::snMsgJoinRequest>(buf, sizeof(buf), &sizeofbuf);
#endif

			spy.cxn->QueueFrame(buf, sizeofbuf, 1u, nullptr);
		}
	}

	void RageConnector::stopSpying(Spy* spy)
	{
		destroyCxn(spy->cxn);
		for (auto i = spies.begin(); i != spies.end(); ++i)
		{
			if (&*i == spy)
			{
				spies.erase(i);
				break;
			}
		}
	}

	void RageConnector::stopAllSpies()
	{
		for (auto& spy : spies)
		{
			destroyCxn(spy.cxn);
		}
	}

	bool Spy::hasEvent() const noexcept
	{
		return !events.empty();
	}

	SpyEvent Spy::popEvent()
	{
		SpyEvent evt = std::move(events.front());
		events.pop_front();
		return evt;
	}

	void Spy::gracefullyStopSpying()
	{
		MsgLostConnectionToHost msg{};
		(*pointers::connection_mgr)->Send(cxn->id, msg, 1u);
	}

	void Spy::fireAddGamer(rage::rlGamerInfo&& gamer)
	{
		SpyEvent evt;
		evt.type = SpyEvent::ADD_GAMER;
		evt.gamerInfo = std::move(gamer);
		events.emplace_back(std::move(evt));
	}

	void Spy::fireRemoveGamer(const rage::rlGamerHandle& gamerHandle)
	{
		SpyEvent evt;
		evt.type = SpyEvent::REMOVE_GAMER;
		evt.gamerInfo.getHandle() = gamerHandle;
		events.emplace_back(std::move(evt));
	}
#endif
}
