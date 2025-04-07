#include "ToastConfig.hpp"

#include "CommandList.hpp"
#include "CommandTogglePointer.hpp"
#include "Util.hpp"

namespace Stand
{
	void ToastConfig::fromToastFlags(toast_t tf) noexcept
	{
		notify = (tf & TOAST_DEFAULT) != 0;
		log_file = (tf & TOAST_FILE) != 0;
		log_console = (tf & TOAST_CONSOLE) != 0;
		chat = (tf & TOAST_CHAT) != 0;
		team_chat = (tf & TOAST_CHAT_TEAM) != 0;
	}

	toast_t ToastConfig::getToastFlags() const noexcept
	{
		toast_t tf = 0;
		if (notify)
		{
			tf = TOAST_DEFAULT;
		}
		if (log_file)
		{
			tf |= TOAST_FILE;
		}
		if (log_console)
		{
			tf |= TOAST_CONSOLE;
		}
		if (chat)
		{
			tf |= TOAST_CHAT;
		}
		else if (team_chat)
		{
			tf |= TOAST_CHAT_TEAM;
		}
		return tf;
	}

	void ToastConfig::populateList(CommandList* list, const std::vector<CommandName>& command_names_prefix)
	{
		list->createChild<CommandTogglePointer>(&notify, LOC("RCT_T"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME("notify")));
		list->createChild<CommandTogglePointer>(&log_file, LOC("RCT_L_F"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME("log")));
		list->createChild<CommandTogglePointer>(&log_console, LOC("RCT_L_C"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME("console")));
		list->createChild<CommandTogglePointer>(&chat, LOC("RCT_C"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME("chat")));
		list->createChild<CommandTogglePointer>(&team_chat, LOC("RCT_C_T"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME("teamchat")));
	}
}
