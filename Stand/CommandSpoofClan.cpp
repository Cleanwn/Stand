#include "CommandSpoofClan.hpp"

#include "CommandExtraInfo.hpp"
#include "ExecCtx.hpp"
#include "get_next_arg.hpp"
#include "Hooking.hpp"
#include "is_session.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "rlClanClient.hpp"
#include "scrClanDesc.hpp"
#include "ScriptGlobal.hpp"
#include "StringUtils.hpp"

#include "CommandSpoofClanToggle.hpp"

#include "CommandSpoofClanId.hpp"
#include "CommandSpoofClanTag.hpp"
#include "CommandSpoofClanName.hpp"
#include "CommandSpoofClanMotto.hpp"
#include "CommandSpoofClanAlt.hpp"
#include "CommandSpoofClanColour.hpp"

#include "CommandSpoofClanPreset.hpp"
#include "CommandSpoofClanMine.hpp"

namespace Stand
{
	CommandSpoofClan::CommandSpoofClan(CommandList* const parent)
		: CommandList(parent, LOC("SPFCRW"))
	{
		instance = this;

		this->createChild<CommandSpoofClanToggle>();
		id = this->createChild<CommandSpoofClanId>();
		tag = this->createChild<CommandSpoofClanTag>();
		name = this->createChild<CommandSpoofClanName>();
		motto = this->createChild<CommandSpoofClanMotto>();
		alt = this->createChild<CommandSpoofClanAlt>();
		colour = this->createChild<CommandSpoofClanColour>();

		{
			auto presets = this->createChild<CommandList>(LOC("PRSTS"));

			presets->createChild<CommandSpoofClanPreset>(0, LOC("NCRW"), "", "", "");
			presets->createChild<CommandSpoofClanPreset>(133742069, "STD", soup::ObfusString("Stand.sh"), "", true);
			presets->createChild<CommandSpoofClanPreset>(42604020, "DEAD", "Runs Dead", "Don't do the time, sub with Twitch Prime");
			presets->createChild<CommandSpoofClanPreset>(735, "IGN", "IGN Crew", "Obsessed with Gaming, Entertainment, and Everything guys enjoy.", true);
		}

		this->createChild<CommandSpoofClanMine>();
	}

	Label CommandSpoofClan::getClanLabel(const char* const tag, const std::string& name, const bool alt_badge)
	{
		std::string str = name;
		str.append(alt_badge ? " < " : " [ ");
		str.append(tag);
		str.append(" ]");
		return LIT(std::move(str));
	}

	void CommandSpoofClan::setValuesFromHooking(Click& click) const
	{
		id->setValueFromHooking(click.type);
		tag->setValueFromHooking(click.type);
		name->setValueFromHooking(click.type);
		motto->setValueFromHooking(click.type);
		alt->setValueFromHooking(click);
		colour->setValueFromHooking(click);

		updateData();
	}

	void CommandSpoofClan::updateData()
	{
		if (is_session_started())
		{
			g_hooking.forceUpdateGamerDataNode();

			ExecCtx::get().ensureScript([]
			{
				scrClanDesc& desc = ScriptGlobal(GLOBAL_CLAN_DESCS).at(g_player, 35).as<scrClanDesc&>();
				rage::rlClanMembershipData& data = *pointers::rage_rlClan_GetPrimaryMembership(0);

				desc.id.Int = (int)data.clan.id;
				desc.r.Int = data.clan.r;
				desc.g.Int = data.clan.g;
				desc.b.Int = data.clan.b;
				strcpy(desc.tag, data.clan.tag);
				strcpy(desc.name, data.clan.name);
				desc.alt_badge.Int = (data.clan.alt_badge ? 1 : 0);
			});
		}
	}
}
