#pragma once

#include "CommandHistoryAction.hpp"

namespace Stand
{
	class CommandHistoryTransInvite : public CommandHistoryAction
	{
	public:
		explicit CommandHistoryTransInvite(CommandList* const parent);

		void onClick(Click& click) final;
	};
}
