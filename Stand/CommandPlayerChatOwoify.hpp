#pragma once

#include "CommandPlayerToggle.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandPlayerChatOwoify : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerChatOwoify(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PLYCHTOWO"), { CMDNAME("chatowo"), CMDNAME("chatuwu") }, LIT(StringUtils::owoify(LANG_GET_W("IPSUM"))), COMMANDPERM_RUDE)
		{
		}

		void onChange(Click& click) final
		{
			g_hooking.owoify_players[getPlayer()] = m_on;
		}
	};
}
