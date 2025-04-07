#pragma once

//#include "NetworkSessionMessages.hpp"
#include "pointers.hpp"
#include "RageConnector.hpp"
#include "RemoteGamer.hpp"
#include "rockstar_admins.hpp"
#include "Script.hpp"
#include "tbLoadingSessionDetails.hpp"
#include "Util.hpp"

namespace Stand
{
	struct RemoteGamerConnector
	{
		static void awaitDetails(RemoteGamer* g)
		{
			g_tb_loading_session_details.enable();
			while (g->isQueuedOrPending())
			{
				Script::current()->yield();
			}
			g_tb_loading_session_details.disable();
		}

		[[nodiscard]] static rage::netEndpoint* establishConnection(const rage::rlSessionInfo& si)
		{
			g_tb_loading_session_details.enable();
			RageConnector::connect(si);
			do
			{
				Script::current()->yield();
			} while (RageConnector::isConnectingTo(si.host.startup_id));
			g_tb_loading_session_details.disable();

			if (!RageConnector::hasConnection(si.host.startup_id))
			{
				Util::toast(LOC("CONFAIL"));
				return nullptr;
			}
			return RageConnector::getConnection(si.host.startup_id);
		}

		[[nodiscard]] static rage::netEndpoint* getDetailsAndEstablishHostConnection(RemoteGamer* g)
		{
			awaitDetails(g);
			if (!g->hasSession())
			{
				Util::toast(LOC("JOIN_T_N"));
				return nullptr;
			}
			return establishConnection(g->session_info);
		}

		/*static void kickViaRid(int64_t rid)
		{
			auto* g = RemoteGamer::get(rid, true, RGU_BULK);
			if (g == nullptr)
			{
				Util::toast(LOC("GENFAIL"));
				return;
			}
			if (auto endpoint = getDetailsAndEstablishHostConnection(g))
			{
				MsgLostConnectionToHost msg;
				msg.session_id = g->session_info.id;
				msg.gamer_handle = rid;
				(*pointers::connection_mgr)->SendOutOfBand(
					endpoint->address,
					RageConnector::getChannelId(g->session_info.host.startup_id),
					msg,
					0
				);
			}
		}*/
	};
}
