#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandSelfChatCommandsFriendsPerms : public CommandToggle
	{
	public:
		explicit CommandSelfChatCommandsFriendsPerms(CommandList* const parent)
			: CommandToggle(parent, LIT_OBF("Self Chat Commands Friends Perms"), CMDNAMES(), NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_TEMPORARY)
		{
		}

		void onChange(Click& click) final
		{
			Click::self_chat_commands_friends_perms = m_on;
		}
	};
}
