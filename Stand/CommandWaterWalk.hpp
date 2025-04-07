#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "gta_ped.hpp"

namespace Stand
{
	class CommandWaterWalk : public CommandToggle
	{
	public:
		explicit CommandWaterWalk(CommandList* const parent)
			: CommandToggle(parent, LOC("WTRWLK"), { CMDNAME("walkunderwater"), CMDNAME("waterwalk") })
		{
		}

		void onEnable(Click& click) final
		{
			if (ENTITY::IS_ENTITY_IN_WATER(g_player_ped))
			{
				g_player_ped.setPos(g_player_ped.getPos());
			}
			registerAsyncTickEventHandler(click, [this]()
			{
				g_player_ped.getCPed()->m_nPhysicalFlags.bIsNotBuoyant = m_on;
				return m_on;
			});
		}
	};
}
