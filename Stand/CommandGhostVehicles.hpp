#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandGhostVehicles : public CommandToggle
	{
	public:
		static inline CommandToggle* instance = nullptr;

		explicit CommandGhostVehicles(CommandList* const parent)
			: CommandToggle(parent, LOC("GHOSTVEHS"), CMDNAMES("ghostvehicles"))
		{
			instance = this;
		}

		~CommandGhostVehicles()
		{
			if (instance == this)
			{
				instance = nullptr;
			}
		}

		void onChange(Click& click) final
		{
			g_gui.no_collision_with_other_vehicles = m_on;
		}
	};
}
