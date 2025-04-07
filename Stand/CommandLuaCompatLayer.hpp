#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandLuaCompatLayer : public CommandListSelect
	{
	public:
		explicit CommandLuaCompatLayer(CommandList* const parent)
			: CommandListSelect(parent, LOC("CMPTLY"), {}, NOLABEL, {
				{0, LOC("NONE")},
				{1, LIT("2Take1Menu")},
				{2, LIT("Kiddion's Modest Menu")},
			}, 0, CMDFLAGS_LIST_SELECT & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			this->parent->as<CommandLuaScript>()->compat_2take1 = (value == 1);
			this->parent->as<CommandLuaScript>()->compat_kiddions = (value == 2);
		}

		void onTickInGameViewport() final
		{
			if (this->parent->as<CommandLuaScript>()->compat_2take1)
			{
				setValueIndication(1);
			}
			else if (this->parent->as<CommandLuaScript>()->compat_kiddions)
			{
				setValueIndication(2);
			}
			else
			{
				setValueIndication(0);
			}
		}

		void onTickInWebViewport() final
		{
			onTickInWebViewportImplRedirect();
		}
	};
}
