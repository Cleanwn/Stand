#pragma once

#include "CommandToggle.hpp"

#include "ScriptFuncPatch.hpp"

namespace Stand
{
	class CommandFullPlayerList : public CommandToggle
	{
	private:
		ScriptFuncPatch patch_1;
		ScriptFuncPatch patch_2;

	public:
		explicit CommandFullPlayerList(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
