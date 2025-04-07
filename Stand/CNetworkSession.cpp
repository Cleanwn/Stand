#include "CNetworkSession.hpp"

#include "netEndpoint.hpp"
#include "pointers.hpp"
#include "SessionType.hpp"

using namespace Stand;

namespace rage
{
	snPeer* snSession::FindPeerByCxn(int32_t id) const
	{
		for (uint32_t i = 0; i != num_peers; ++i)
		{
			if (peers[i]->cxn_id == id)
			{
				return peers[i];
			}
		}
		return nullptr;
	}

	snPeer* snSession::FindPeerById(netPeerId peerId) const
	{
		for (uint32_t i = 0; i != num_peers; ++i)
		{
			if (peers[i]->peer_info.id == peerId)
			{
				return peers[i];
			}
		}
		return nullptr;
	}

	/*void netPeerComplainer::sendComplaint(const netComplaintMsg* packet) noexcept
	{
		if (auto endpoint = con_mgr->getEndpointById(host_endpoint))
		{
			pointers::send_netComplaintMsg(con_mgr, endpoint->GetAddress(), channel_index, packet, 0);
		}
	}*/
}

void CBlacklistedGamers::AddGamer(const rage::rlGamerHandle& gh) noexcept
{
	return pointers::CBlacklistedGamers_BlacklistGamer(this, gh, /* BLACKLIST_VOTED_OUT */ 1);
}

bool CBlacklistedGamers::IsBlacklisted(const rage::rlGamerHandle& target) const noexcept
{
	for (const BlacklistEntry* it = first; it; it = it->next)
	{
		if (it->gamer_handle == target)
		{
			return true;
		}
	}
	return false;
}

void CNetworkSession::setSlots(Stand::SessionType type) noexcept
{
	switch (type)
	{
	case SOLO:
		session->rline_session.data.config.max_public_slots = 0;
		session->rline_session.data.config.max_private_slots = 2;
		break;

	case INVITE_ONLY:
		session->rline_session.data.config.max_public_slots = 0;
		session->rline_session.data.config.max_private_slots = MAX_PLAYERS;
		break;

	default:
		session->rline_session.data.config.max_public_slots = MAX_PLAYERS;
		session->rline_session.data.config.max_private_slots = 0;
		break;
	}

	m_Config[ST_Physical].m_MaxSlots = (session->rline_session.data.config.max_public_slots + session->rline_session.data.config.max_private_slots);
	m_Config[ST_Physical].m_MaxPrivSlots = session->rline_session.data.config.max_private_slots;

	setMatchmakingGroupSlots(type);
}

void CNetworkSession::setMatchmakingGroupSlots(Stand::SessionType type) noexcept
{
	switch (type)
	{
	case SOLO:
		m_MatchmakingGroupMax[0] = 1;
		m_MatchmakingGroupMax[4] = 1;
		break;

	default:
		m_MatchmakingGroupMax[0] = (MAX_PLAYERS - NUM_SPECTATOR_SLOTS);
		m_MatchmakingGroupMax[4] = NUM_SPECTATOR_SLOTS;
		break;
	}
}
