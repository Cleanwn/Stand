#pragma once

#include "CommandToggle.hpp"

#include "ScriptFuncPatch.hpp"

namespace Stand
{
	class CommandNoDailyExpenses : public CommandToggle
	{
	private:
		ScriptFuncPatch patch;

	public:
		explicit CommandNoDailyExpenses(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
