#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandSpoofClanAlt : public CommandToggle
	{
	public:
		explicit CommandSpoofClanAlt(CommandList* const parent)
			: CommandToggle(parent, LOC("ALTBDG"), { CMDNAME("crewaltbadge") }, NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			g_hooking.spoofed_clan_membership.clan.alt_badge = m_on;
			if (g_hooking.spoof_clan_membership)
			{
				parent->as<CommandSpoofClan>()->updateData();
			}
			setValueFromHooking(click);
		}

		void setValueFromHooking(Click& click)
		{
			setStateBool(click, g_hooking.spoofed_clan_membership.clan.alt_badge);
		}
	};
}
