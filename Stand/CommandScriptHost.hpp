#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandScriptHost : public CommandAction
	{
	public:
		explicit CommandScriptHost(CommandList* const parent);

		void onClick(Click& click) final;
	};
}
