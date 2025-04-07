#include "conf_netcode.hpp"
#if I_CAN_SPY

#include "CommandListSessionMember.hpp"

#include <soup/ObfusString.hpp>

#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandListSession.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "natives.hpp"
#include "NetworkSessionMessages.hpp"
#include "pointers.hpp"
#include "RageConnector.hpp"
#include "rlSessionInfo.hpp"
#include "Script.hpp"
#include "Util.hpp"

#if SPY_CAN_KICK
#include "CommandListSessionMemberOrgasm.hpp"
#endif

namespace Stand
{
	[[nodiscard]] static Label getHelpText(const rage::rlGamerInfo& gamerInfo)
	{
		std::string help_text = soup::ObfusString("Rockstar ID: ").str();
		help_text.append(fmt::to_string(gamerInfo.getHandle().rockstar_id));
		help_text.push_back('\n');
		help_text.append(LANG_GET("HOSTKN"));
		help_text.append(": ");
		help_text.append(Util::to_padded_hex_string(gamerInfo.peer.id));
		return LIT(std::move(help_text));
	}

	CommandListSessionMember::CommandListSessionMember(CommandList* const _parent, rage::rlGamerInfo&& _gamerInfo)
		: CommandList(_parent, LIT("RID "), {}, getHelpText(_gamerInfo), CMDFLAGS_LIST | CMDFLAG_TEMPORARY), gamerInfo(std::move(_gamerInfo)), account(ScAccount::fromRID(gamerInfo.getHandle().rockstar_id)), last_accounted_for(get_current_time_millis())
	{
		menu_name.literal_str.append(fmt::to_string(gamerInfo.getHandle().rockstar_id));

		this->createChild<CommandLambdaActionScript>(LOC("PLYPRFL"), {}, NOLABEL, [this](const Click& click)
		{
			int network_handle[13 * 2] = { 0 };
			std::string rid_str = fmt::to_string(gamerInfo.getHandle().rockstar_id);
			NETWORK::NETWORK_HANDLE_FROM_MEMBER_ID(rid_str.c_str(), (Any*)network_handle, 13);
			g_gui.inputStop();
			NETWORK::NETWORK_SHOW_PROFILE_UI((Any*)network_handle);
		}, CMDFLAG_TEMPORARY);

#if SPY_CAN_KICK
		//this->createChild<CommandDivider>(LOC("PLYKCK"));
#if false // breakup doesn't work from non hosts anymore
		this->createChild<CommandLambdaAction>(LOC("PLYKCK_B"), {}, NOLABEL, [this](Click& click)
		{
			if (click.isUltimateEdition())
			{
				if (auto si = getSessionInfoIfAvailable())
				{
					if (auto endpoint = RageConnector::getConnection(si->host.startup_id))
					{
						endpoint->removeGamer(
							RageConnector::getChannelId(si->host.startup_id),
							si->id,
							gamerInfo.getHandle()
						);
					}
				}
			}
		}, CMDFLAG_TEMPORARY);
#endif
		this->createChild<CommandListSessionMemberOrgasm>();
#endif
	}

	const CommandListSession* CommandListSessionMember::getSession() const
	{
		return parent->as<CommandListSession>();
	}

	const rage::rlSessionInfo* CommandListSessionMember::getSessionInfoIfAvailable() const
	{
		return getSession()->getSessionInfoIfAvailable();
	}

	void CommandListSessionMember::onTickInGameViewport()
	{
		if (!did_scaccount_lookup)
		{
			if (account->isComplete())
			{
				did_scaccount_lookup = true;
				if (account->hasName())
				{
					setMenuName(LIT(account->name));
				}
			}
			else
			{
				if (!account->wasCompletionRequested())
				{
					account->requestCompletionPreferCache();
				}
			}
		}
	}

	void CommandListSessionMember::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
#endif
