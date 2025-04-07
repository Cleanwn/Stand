#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandBanBypass : public CommandAction
	{
	public:
		explicit CommandBanBypass(CommandList* parent);

		void onClick(Click& click) final;
	};
}
