#pragma once

#include "CommandListAction.hpp"

namespace Stand
{
	class CommandSetFont : public CommandListAction
	{
	public:
		explicit CommandSetFont(CommandList* parent);

		void onClick(Click& click) final;

		void onItemClick(Click& click, CommandListActionItem* item) final;
	};
}
