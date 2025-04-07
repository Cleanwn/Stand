#include "CNetworkPlayerMgr.hpp"

#include "CNetGamePlayer.hpp"
#include "hooking.hpp"

namespace rage
{
	bool netPlayerMgrBase::SendBuffer(const rage::netPlayer* player, const void* data, unsigned int size, unsigned int sendFlags, netSequence* seq, const rage::netPlayer* sender)
	{
		//return (*pointers::connection_mgr)->Send(player->channel_7_cxn_id, data, size, sendFlags, seq);
		return OG(rage_netPlayerMgrBase_SendBuffer)(this, player, data, size, sendFlags, seq, sender);
	}
}

compactplayer_t CNetworkPlayerMgr::getHostIndex() const
{
	compactplayer_t p = 0;
	for (; p != MAX_PLAYERS; ++p)
	{
		if (Players[p] != nullptr
			&& Players[p]->isHost()
			)
		{
			break;
		}
	}
	return p;
}
