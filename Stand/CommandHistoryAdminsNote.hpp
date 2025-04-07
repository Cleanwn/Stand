#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandHistoryAdminsNote : public CommandAction
	{
	public:
		explicit CommandHistoryAdminsNote(CommandList* const parent);

		std::wstring getCommandSyntax() const final;

		void onCommand(Click& click, std::wstring& args) final;
	};
}
