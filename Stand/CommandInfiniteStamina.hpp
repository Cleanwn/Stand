#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandInfiniteStamina : public CommandToggle
	{
	public:
		explicit CommandInfiniteStamina(CommandList* const parent)
			: CommandToggle(parent, LOC("INFSTAM"), { CMDNAME("infstamina"), CMDNAME("infinitestamina") })
		{
		}

		void onEnable(Click& click) final
		{
			registerAsyncTickEventHandler(click, [this]()
			{
				if (!m_on)
				{
					return false;
				}
				g_player_ped.getPlayerInfo()->stamina = 100.0f;
				return true;
			});
		}
	};
}
