#pragma once

#include "CommandColour.hpp"

namespace Stand
{
	class CommandSpoofClanColour : public CommandColour
	{
	public:
		explicit CommandSpoofClanColour(CommandList* const parent)
			: CommandColour(parent, LOC("CLR"), { CMDNAME("crewcolour") }, NOLABEL, 255, 0, 255, -1)
		{
		}

		void onChange(Click& click) final
		{
			int r, g, b;
			getRGB(r, g, b);
			g_hooking.spoofed_clan_membership.clan.r = r;
			g_hooking.spoofed_clan_membership.clan.g = g;
			g_hooking.spoofed_clan_membership.clan.b = b;
			if (g_hooking.spoof_clan_membership)
			{
				parent->as<CommandSpoofClan>()->updateData();
			}
		}

		void setValueFromHooking(Click& click)
		{
			set(click, g_hooking.spoofed_clan_membership.clan.r, g_hooking.spoofed_clan_membership.clan.g, g_hooking.spoofed_clan_membership.clan.b);
		}
	};
}
