#pragma once

#include "CommandToggle.hpp"

#include "is_session.hpp"

namespace Stand
{
	class CommandPassiveWeaponizedVehicles : public CommandToggle
	{
	public:
		explicit CommandPassiveWeaponizedVehicles(CommandList* const parent)
			: CommandToggle(parent, LOC("PASSIWPNVEH"), { CMDNAME("passiweaponveh") })
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (is_session_started())
				{
					*ScriptGlobal(GLOBAL_TUNABLE_BASE + GLOBAL_TUNABLE_ALLOW_WEAPONIZED_VEHICLES_IN_PASSIVE_MODE).as<BOOL*>() = m_on;
				}
				return m_on;
			});
		}
	};
}
