#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDebugNatives : public CommandToggle
	{
	public:
		explicit CommandDebugNatives(CommandList* const parent);

		void onEnable(Click& click) final;

		void onDisable(Click& click) final;
	};
}
