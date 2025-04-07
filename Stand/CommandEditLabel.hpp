#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandEditLabel : public CommandAction
	{
	public:
		explicit CommandEditLabel(CommandList* const parent);

		void onClick(Click& click) final;
		void onCommand(Click& click, std::wstring& args) final;

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final;
	};
}
