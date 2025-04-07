#pragma once

#include "CommandToggle.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandNoDlcIntro : public CommandToggle
	{
	public:
		explicit CommandNoDlcIntro(CommandList* const parent)
			: CommandToggle(parent, LOC("NDLCITRO"), { CMDNAME("nodlcintromovie") })
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_DISABLE_DLC_INTRO_MOVIE).as<int*>() = 1;
			});
		}
	};
}
