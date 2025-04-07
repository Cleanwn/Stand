#pragma once

#include "CommandListAction.hpp"

namespace Stand
{
	class CommandSessionType : public CommandListAction
	{
	public:
		explicit CommandSessionType(CommandList* const parent);

		void onItemClick(Click& click, CommandListActionItem* item) final;
	};
}
