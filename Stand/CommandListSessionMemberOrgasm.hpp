#pragma once

#include "CommandListSessionMemberAction.hpp"

#include "netConnectionManager.hpp"
#include "NetworkSessionMessages.hpp"
#include "Script.hpp"
//#include "Tunables.hpp"
#include "tunneler.hpp"

namespace Stand
{
	class CommandListSessionMemberOrgasm : public CommandListSessionMemberAction
	{
	private:
		bool busy = false;

	public:
		explicit CommandListSessionMemberOrgasm(CommandList* const parent)
			: CommandListSessionMemberAction(parent, /*LOC("PLYKCK_O")*/ /*LOC("PLYKCK")*/ LIT("[Debug Build] Kick"), {}, LOC("PLYKCK_H_B_P"), CMDFLAG_TEMPORARY)
		{
		}

		void onClick(Click& click) final
		{
			/*if (!g_tunables.remote_kick_unlocked)
			{
				click.setResponse(LOC("CMDDISA"));
				return;
			}*/
			if (busy || AbstractPlayer::isOrgasmKickOnCooldown())
			{
				return click.setResponse(LOC("2FAST"));
			}
			if (!click.isUltimateEdition())
			{
				return;
			}
			busy = true;
			ensureYieldableScriptThread([this]
			{
				if (auto si = getSessionInfoIfAvailable())
				{
					const rage::netPeerAddress& orgasm_target = getMember()->gamerInfo.peer.address;
					const auto channelId = RageConnector::getChannelId(si->host.startup_id);

					if (orgasm_target.startup_id == si->host.startup_id)
					{
						if (auto spy = RageConnector::spyFindSessionId(si->id))
						{
							MsgRadioStationSyncRequest msg;
							for (int i = 0; i != 2000; ++i)
							{
								(*pointers::connection_mgr)->Send(spy->cxn->id, msg, 1u);

								// Does not seem to work out-of-band:
								//(*pointers::connection_mgr)->SendOutOfBand(spy->cxn->endpoint->GetAddress(), channelId, msg, 1u);
							}
						}
					}
					else
					{
#if false
						using netTunnelRequest_ctor_t = void(__fastcall*)(void*);
						char tunnelRqst[0x1000];
						((netTunnelRequest_ctor_t)IDA_ADDR(00007FF72569DA28))(tunnelRqst);
#else
						rage::netTunnelRequest tunnelRqst;
#endif

						rage::netTunnelDesc tunnelDesc(si->token);
						rage::netStatus status{};
						pointers::rage_netConnectionManager_OpenTunnel(*pointers::connection_mgr, orgasm_target.gamer_handle, orgasm_target, tunnelDesc, &tunnelRqst, &status);
						while (status.isPending())
						{
							Script::current()->yield();
						}
						//Util::toast(fmt::format("OpenTunnel finished with status {}", status.m_Status));

						if (auto endpoint = (*pointers::connection_mgr)->getEndpointByStartupId(orgasm_target.startup_id))
						{
							//Util::toast("Found endpoint");

							auto cxn = pointers::rage_netConnectionManager_CreateConnection(*pointers::connection_mgr, endpoint, channelId);

							MsgRadioStationSyncRequest msg;
							for (int i = 0; i != 2000; ++i)
							{
								(*pointers::connection_mgr)->Send(cxn->id, msg, 1u);
							}

							Script::current()->yield(3000);
							//Util::toast("Destroying cxn");
							RageConnector::destroyCxn(cxn);
						}
					}
					AbstractPlayer::next_orgasm_kick = get_current_time_millis() + 20'000;
					busy = false;
				}
			});
		}
	};
}
