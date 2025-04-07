#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "ChatCommandsCommon.hpp"
#include "evtChatEvent.hpp"

namespace Stand
{
	class CommandChatCommandsStrangers : public CommandToggle
	{
	public:
		explicit CommandChatCommandsStrangers(CommandList* const parent)
			: CommandToggle(parent, LOC("ON"), { CMDNAME("commandsforstrangers"), CMDNAME("chatcommandsforstrangers") })
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
			if (e.sender != g_player
				&& !AbstractPlayer(e.sender).isFriend()
				&& !AbstractPlayer(e.sender).isCrewMember()
				)
			{
				ChatCommandsCommon::processMessageFromSenderWithChatCommandPermissions(e);
			}
		}
	};
}
