#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "gta_ped.hpp"

namespace Stand
{
	class CommandPedMoreStickies : public CommandToggle
	{
	public:
		explicit CommandPedMoreStickies(CommandList* const parent)
			: CommandToggle(parent, LOC("INCSTICK"), {}, LOC("INCPROJ_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				SOUP_IF_LIKELY (auto p = g_player_ped.getCPed())
				{
					p->m_uStickyCount = 0;
				}
			});
		}
	};
}
