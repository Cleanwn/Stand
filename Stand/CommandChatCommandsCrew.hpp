#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "ChatCommandsCommon.hpp"
#include "evtChatEvent.hpp"

namespace Stand
{
	class CommandChatCommandsCrew : public CommandToggle
	{
	public:
		explicit CommandChatCommandsCrew(CommandList* const parent)
			: CommandToggle(parent, LOC("ON"), { CMDNAME("commandsforcrew"), CMDNAME("chatcommandsforcrew") })
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
			if (AbstractPlayer(e.sender).isCrewMember())
			{
				ChatCommandsCommon::processMessageFromSenderWithChatCommandPermissions(e);
			}
		}
	};
}
