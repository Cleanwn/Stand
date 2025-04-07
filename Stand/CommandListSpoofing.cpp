#include "CommandListSpoofing.hpp"

#include "CommandTogglePointer.hpp"
#include "ComponentSpoofPos.hpp"

#include "CommandSpoofName.hpp"
#include "CommandSpoofedName.hpp"
#include "CommandSpoofedNameFromHistory.hpp"

#if false
#include "CommandSpoofPresets.hpp"
#endif

#if false
#include "CommandSpoofHostToken.hpp"
#include "CommandSpoofedHostToken.hpp"
#include "CommandHostTokenPresets.hpp"
#include "CommandAutoKickHost.hpp"
#endif

#include "CommandListSessionSpoof.hpp"
#include "CommandSpoofClan.hpp"
#include "CommandSpoofRank.hpp"
#include "CommandSpoofKd.hpp"

#include "CommandSpoofedPosition.hpp"

#include "CommandDevFlag.hpp"
#include "CommandStreamerSpoof.hpp"
#include "CommandHideFromPlayerList.hpp"
#include "CommandPhoneModeSpoof.hpp"

namespace Stand
{
	CommandListSpoofing::CommandListSpoofing(CommandList* const parent)
		: CommandList(parent, LOC("SPF"))
	{
		auto name_spoofing = this->createChild<CommandList>(LOC("SPFNME_D"), {}, LOC("GEN_H_LOC"));
		name_spoofing->createChild<CommandSpoofName>();
		name_spoofing->createChild<CommandSpoofedName>();
		name_spoofing->createChild<CommandSpoofedNameFromHistory>();

#if false
		this->createChild<CommandSpoofPresets>();
#endif

#if false
		{
			auto host_token_spoofing = this->createChild<CommandList>(LOC("SPFHT_D"), {}, LOC("SPFHT_H"));
			auto spoof_host_token = host_token_spoofing->createChild<CommandSpoofHostToken>();
			auto spoofed_host_token = host_token_spoofing->createChild<CommandSpoofedHostToken>(spoof_host_token);
			host_token_spoofing->createChild<CommandHostTokenPresets>(spoofed_host_token);
			{
				auto kick_host_list = host_token_spoofing->createChild<CommandList>(LOC("AKCKHST"));
				auto kick_host = kick_host_list->createChild<CommandAutoKickHost>();
				kick_host->excludes.populateList(kick_host_list, {}, PlayerExcludes::SHOW_MODDER | PlayerExcludes::SHOW_ORG_MEMBERS | PlayerExcludes::SHOW_LIKELY_MODDER);
			}
		}
#endif

		this->createChild<CommandListSessionSpoof>();
		this->createChild<CommandSpoofClan>();
		this->createChild<CommandSpoofRank>();
		this->createChild<CommandSpoofKd>();
		{
			auto pos_spoofing = this->createChild<CommandList>(LOC("SPFPOS"));

			pos_spoofing->createChild<CommandTogglePointer>(&g_comp_spoofpos.spoof_position, LOC("SPFPOS"), CMDNAMES_OBF("spoofpos"));
			pos_spoofing->createChild<CommandSpoofedPosition>();
		}
		this->createChild<CommandDevFlag>();
		this->createChild<CommandTogglePointer>(&g_hooking.spoof_qa_flag, LOC("MDDRDT_RSQA"), { CMDNAME("qaflag") }, LOC("SPF_H_FLAG"));
		this->createChild<CommandTogglePointer>(&g_hooking.spoof_cheater_flag, LOC("MDDRDT_CHEAT"), { CMDNAME("cheaterflag") }, LOC("SPF_H_FLAG"));
		if (g_gui.root_state != GUI_MINIMAL)
		{
			this->createChild<CommandStreamerSpoof>();
			this->createChild<CommandHideFromPlayerList>();
		}
		this->createChild<CommandPhoneModeSpoof>();
	}
}
