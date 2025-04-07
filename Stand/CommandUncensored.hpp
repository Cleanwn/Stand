#pragma once

#include "CommandListSelect.hpp"

#include "CMultiplayerChat.hpp"

namespace Stand
{
	class CommandUncensored : public CommandListSelect
	{
	public:
		explicit CommandUncensored(CommandList* const parent)
			: CommandListSelect(parent, LOC("PRFNTY"), { CMDNAME("uncensored") }, LOC("PRFNTY_H"), {
				{ CMultiplayerChat::CLOSED, LOC("DOFF"), { CMDNAME("off") } },
				{ CMultiplayerChat::ALL, LOC("ON"), { CMDNAME("on") } },
				{ CMultiplayerChat::TEAM, LOC("ONLYTEAM"), { CMDNAME("team") } }
			}, CMultiplayerChat::CLOSED)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			g_hooking.bypass_profanity_filter = (this->value != CMultiplayerChat::CLOSED);
			g_hooking.bypass_profanity_filter_only_team = (this->value == CMultiplayerChat::TEAM);
		}
	};
}
