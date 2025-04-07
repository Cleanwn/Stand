#if false // needs some work before it will compile again
#ifdef STAND_DEBUG

#include "CommandHistoryTransInvite.hpp"

#include "CNetworkSession.hpp"
#include "ComponentNetcode.hpp"
#include "HistoricPlayer.hpp"
#include "RemoteGamerConnector.hpp"
#include "natives.hpp"
#include "Script.hpp"

namespace Stand
{
	CommandHistoryTransInvite::CommandHistoryTransInvite(CommandList* const parent)
		: CommandHistoryAction(parent, LIT_OBF("[Debug Build] Invite Via Quickmatch"), {}, LIT_OBF("Target will need to accept the invite. Their hud will be broken & they won't be able to chat with people who joined normally."))
	{
	}

	void CommandHistoryTransInvite::onClick(Click& click)
	{
		uint64_t target_rid = getHp()->rid;
		ensureYieldableScriptThread(click, [target_rid]
		{
			// Get details
			auto* g = RemoteGamer::get(target_rid, true, RGU_DETAILED);
			if (g == nullptr)
			{
				Util::toast(LOC("GENFAIL"));
				return;
			}
			RemoteGamerConnector::awaitDetails(g);

			// Establish connection
			if (!g->hasPeerAddr())
			{
				Util::toast(LOC("PLYOFFLINE"));
				return;
			}
			auto endpoint = RemoteGamerConnector::establishConnection(g->peeraddr);
			if (!endpoint)
			{
				Util::toast(LOC("PLYOFFLINE"));
				return;
			}

			// Start transition session
			/*if (NETWORK::NETWORK_IS_IN_TRANSITION())
			{
				NETWORK::NETWORK_BAIL_TRANSITION(0, 0, 0);
			}
			else*/
			if (!NETWORK::NETWORK_IS_IN_TRANSITION())
			{
				NETWORK::NETWORK_HOST_TRANSITION(0, 32, 200, 13, 0, 0, 0, 0, 0, 0);
				while (NETWORK::NETWORK_IS_TRANSITION_BUSY())
				{
					Script::current()->yield();
				}
			}

			// Send quickmatch invite
			{
				MsgTransitionToActivityStart msg;
				msg.m_hGamer = target_rid;
				msg.m_bIsAsync = true;
				(*pointers::connection_mgr)->SendOutOfBand(endpoint->address, (*pointers::network_session)->transition->channel_id, msg, 0);
			}
			{
				MsgTransitionToActivityFinish msg;
				msg.m_hGamer = target_rid;
				msg.m_bDidSucceed = true;
				msg.m_bIsPrivate = false;
				msg.m_SessionInfo = (*pointers::network_session)->transition->rline_session.data.info;
				(*pointers::connection_mgr)->SendOutOfBand(endpoint->address, (*pointers::network_session)->transition->channel_id, msg, 0);
			}

			Util::toast(LIT_OBF("Waiting for them to accept the invite..."));
			g_comp_netcode.trans_target_acquired = false;
			while (!g_comp_netcode.trans_target_acquired)
			{
				Script::current()->yield();
			}

			Util::toast(LIT_OBF("Hook..."));
			{
				MsgTransitionLaunch msg{};
				(*pointers::connection_mgr)->SendOutOfBand(endpoint->address, (*pointers::network_session)->transition->channel_id, msg, 0);
			}
			g_comp_netcode.trans_target_lost = false;
			while (!g_comp_netcode.trans_target_lost)
			{
				Script::current()->yield();
			}

			Util::toast(LIT_OBF("Line..."));
			Script::current()->yield(1000);

			// They are in an activity session now, we can instruct them to join us.
			Util::toast(LIT_OBF("Sinker..."));
			{
				MsgTransitionToGameStart msg;
				msg.m_hGamer = 1337;
				msg.m_GameMode = 0;
				msg.m_IsTransitionFromLobby = false;
				(*pointers::connection_mgr)->SendOutOfBand(endpoint->address, (*pointers::network_session)->transition->channel_id, msg, 0);
			}
			{
				MsgTransitionToGameNotify msg;
				msg.m_hGamer = 1337;
				msg.m_NotificationType = MsgTransitionToGameNotify::NOTIFY_SWAP;
				msg.m_SessionInfo = (*pointers::network_session)->session->rline_session.data.info;
				msg.m_bIsPrivate = true;
				(*pointers::connection_mgr)->SendOutOfBand(endpoint->address, (*pointers::network_session)->transition->channel_id, msg, 0);
			}
		});
	}
}

#endif
#endif
