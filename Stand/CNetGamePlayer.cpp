#include "CNetGamePlayer.hpp"

#include "CPlayerInfo.hpp"
#include "netConnectionManager.hpp"
#include "pointers.hpp"

namespace rage
{
	const rlGamerInfo* netPlayer::GetGamerInfoImpl() const
	{
		return reinterpret_cast<const CNetGamePlayer*>(this)->GetGamerInfoImpl();
	}

	rage::netEndpoint* netPlayer::getEndpoint() const
	{
		return (*Stand::pointers::connection_mgr)->getEndpointById(endpoint_id);
	}
}

rage::rlGamerInfo* CNetGamePlayer::GetGamerInfoImpl()
{
	return &player_info->gamer_info;
}

const rage::rlGamerInfo* CNetGamePlayer::GetGamerInfoImpl() const
{
	return &player_info->gamer_info;
}

bool CNetGamePlayer::isHost() const
{
	return GetGamerInfoImpl()->isHost();
}
