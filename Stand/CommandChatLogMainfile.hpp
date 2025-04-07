#pragma once

#include "CommandToggle.hpp"

#include "evtChatEvent.hpp"
#include "FileLogger.hpp"

namespace Stand
{
	class CommandChatLogMainfile : public CommandToggle
	{
	private:
		static void onChatEvent(evtChatEvent& e)
		{
			g_logger.log(e.getLogEntry());
		}

	public:
		explicit CommandChatLogMainfile(CommandList* const parent)
			: CommandToggle(parent, LIT("Log.txt"))
		{
		}

		void onEnable(Click& click) final
		{
			evtChatEvent::registerHandler(&onChatEvent);
		}

		void onDisable(Click& click) final
		{
			evtChatEvent::unregisterHandler(&onChatEvent);
		}
	};
}
