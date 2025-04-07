#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandSetCarClubRepLevel : public CommandAction
	{
	public:
		explicit CommandSetCarClubRepLevel(CommandList* parent);

		std::wstring getCommandSyntax() const final;

		void onClick(Click& click) final;

		void onCommand(Click& click, std::wstring& args) final;
	};
}
