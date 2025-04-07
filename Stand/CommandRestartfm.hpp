#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandRestartfm : public CommandAction
	{
	public:
		explicit CommandRestartfm(CommandList* const parent);

		void onClick(Click& click) final;
	};
}
