#pragma once

#include <CommandAction.hpp>

namespace Stand
{
	class CommandAddLabel : public CommandAction
	{
	public:
		explicit CommandAddLabel(CommandList* const parent);

		void onCommand(Click& click, std::wstring& args) final;
	};
}
