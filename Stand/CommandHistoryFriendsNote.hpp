#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandHistoryFriendsNote : public CommandAction
	{
	public:
		explicit CommandHistoryFriendsNote(CommandList* const parent);

		std::wstring getCommandSyntax() const final;

		void onCommand(Click& click, std::wstring& args) final;
	};
}
