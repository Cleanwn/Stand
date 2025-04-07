#pragma once

#include "CommandListAction.hpp"

namespace Stand
{
	class CommandSetGameLanguage : public CommandListAction
	{
	public:
		explicit CommandSetGameLanguage(CommandList* const parent);

		void onItemClick(Click& click, CommandListActionItem* item) final;
	};
}
