#pragma once

#include "CommandToggle.hpp"

#include "ScriptFuncPatch.hpp"

namespace Stand
{
	class CommandNoLscBarriers : public CommandToggle
	{
	private:
		ScriptFuncPatch patch_1;
		ScriptFuncPatch patch_2;

	public:
		explicit CommandNoLscBarriers(CommandList* parent);

		void onEnable(Click& click) final;
	};
}
