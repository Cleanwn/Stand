#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "ChatCommandsCommon.hpp"
#include "evtChatEvent.hpp"

namespace Stand
{
	class CommandChatCommandsSelf : public CommandToggle
	{
	public:
		explicit CommandChatCommandsSelf(CommandList* const parent)
			: CommandToggle(parent, LOC("ON4M"), CMDNAMES("commandsforme", "commandsforself", "chatcommandsforme", "chatcommandsforself"))
		{
		}

		void onEnable(Click& click) final
		{
			evtChatEvent::registerHandler(&onChat);
		}

		void onDisable(Click& click) final
		{
			evtChatEvent::unregisterHandler(&onChat);
		}

	private:
		static void onChat(evtChatEvent& e)
		{
			if (e.sender == g_player)
			{
				ChatCommandsCommon::processMessageFromSenderWithChatCommandPermissions(e);
			}
		}
	};
}
