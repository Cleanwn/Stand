#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandSendChatMessage : public CommandAction
	{
	private:
		CommandToggleNoCorrelation* const in_team_chat;
		CommandToggleNoCorrelation* const add_to_local_history;

	public:
		explicit CommandSendChatMessage(CommandList* parent, CommandToggleNoCorrelation* in_team_chat, CommandToggleNoCorrelation* add_to_local_history);
		
		std::wstring getCommandSyntax() const final;

		void onCommand(Click& click, std::wstring& args) final;
	};
}
