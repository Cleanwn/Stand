#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "ChatCommandsCommon.hpp"
#include "evtChatEvent.hpp"

namespace Stand
{
	class CommandChatCommandsFriends : public CommandToggle
	{
	public:
		explicit CommandChatCommandsFriends(CommandList* const parent)
			: CommandToggle(parent, LOC("ON"), { CMDNAME("commandsforfriends"), CMDNAME("chatcommandsforfriends") })
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
			if (AbstractPlayer(e.sender).isFriend())
			{
				ChatCommandsCommon::processMessageFromSenderWithChatCommandPermissions(e);
			}
		}
	};
}
