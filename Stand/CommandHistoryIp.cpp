#if false
#ifdef STAND_DEBUG

#include "CommandHistoryIp.hpp"

#include "HistoricPlayer.hpp"
#include "RageConnector.hpp"
#include "rage_netcode.hpp"
#include "RemoteGamer.hpp"

namespace Stand
{
	CommandHistoryIp::CommandHistoryIp(CommandList* const parent, HistoricPlayer* const hp)
		: CommandReadonlyValueCopy(parent, LOC("IP")), hp(hp)
	{
	}

	void CommandHistoryIp::onTickInGameViewport()
	{
		auto g = RemoteGamer::get(hp->rid, true, RGU_DETAILED);
		if (!g)
		{
			return setValue(L"[No Valid RID]");
		}
		if (!g->hasPeerAddr())
		{
			return setValue(L"[Offline]");
		}
		if (!RageConnector::hasConnection(g->peeraddr.startup_id))
		{
			RageConnector::connect(g->peeraddr);
			return setValue(L"[Connecting]");
		}
		return setValue(RageConnector::getConnection(g->peeraddr.startup_id)->getRemoteAddress().ip.toString<std::wstring>());
	}
}

#endif
#endif
