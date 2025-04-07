#pragma once

#include "CommandListSelect.hpp"

#include "CMultiplayerChat.hpp"

namespace Stand
{
	class CommandHtml : public CommandListSelect
	{
	public:
		explicit CommandHtml(CommandList* const parent)
			: CommandListSelect(parent, LOC("HTML"), { CMDNAME("html") }, LOC("HTML_H"), {
				{ CMultiplayerChat::CLOSED, LOC("DOFF"), { CMDNAME("off") } },
				{ CMultiplayerChat::ALL, LOC("ON"), { CMDNAME("on") } },
				{ CMultiplayerChat::TEAM, LOC("ONLYTEAM"), { CMDNAME("team") } }
			}, CMultiplayerChat::CLOSED)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			const bool is_on = this->value != CMultiplayerChat::CLOSED;
			if (g_hooking.is_chat_character_valid_hook.isHooked())
			{
				g_hooking.bypass_character_filter = is_on;
				g_hooking.bypass_character_filter_only_team = this->value == CMultiplayerChat::TEAM;
			}
			else // Hook is missing?
			{
				if (is_on) // User wants to turn it on?
				{
					click.setResponse(LIT(LANG_FMT("HKFAIL", LANG_GET("HTML"))));
				}
			}
		}
	};
}
