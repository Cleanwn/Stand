#pragma once

#include "CommandToggle.hpp"

#include "pointers.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	template <int... globals>
	class CommandKeepOnlineGlobalAtZero : public CommandToggle
	{
	public:
		using CommandToggle::CommandToggle;

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (*pointers::is_session_started)
				{
					constexpr int globals_arr[] = { globals... };
					for (const int global : globals_arr)
					{
						*ScriptGlobal(global).as<int*>() = 0;
					}
				}
			});
		}
	};
}
