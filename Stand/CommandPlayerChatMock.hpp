#pragma once

#include "CommandPlayerToggle.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandPlayerChatMock : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerChatMock(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PLYCHTMCK"), { CMDNAME("chatmock") }, LIT(StringUtils::mock(LANG_GET("IPSUM"))), COMMANDPERM_RUDE)
		{
		}

		void onChange(Click& click) final
		{
			g_hooking.mock_players[getPlayer()] = m_on;
		}
	};
}
