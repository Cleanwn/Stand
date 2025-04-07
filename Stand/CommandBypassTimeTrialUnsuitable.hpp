#pragma once

#include "CommandToggle.hpp"

#include "ScriptFuncPatch.hpp"

namespace Stand
{
	class CommandBypassTimeTrialUnsuitable : public CommandToggle
	{
	private:
		ScriptFuncPatch patch;

	public:
		explicit CommandBypassTimeTrialUnsuitable(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
