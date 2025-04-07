#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandSpoofClanToggle : public CommandToggle
	{
	public:
		explicit CommandSpoofClanToggle(CommandList* const parent)
			: CommandToggle(parent, LOC("SPFCRW"), { CMDNAME("crew") })
		{
		}

		void onChange(Click& click) final
		{
			g_hooking.spoof_clan_membership = m_on;
			parent->as<CommandSpoofClan>()->updateData();
		}
	};
}
