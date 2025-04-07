#pragma once

#include "CommandToggle.hpp"

#include "ManagedTunables.hpp"

namespace Stand
{
	class CommandNoOrbCooldown : public CommandToggle
	{
	public:
		explicit CommandNoOrbCooldown(CommandList* const parent)
			: CommandToggle(parent, LOC("NOORBCLDN"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAME("orbcooldown")))
		{
			command_names.emplace_back(CMDNAME("norbcooldown"));
		}

		void onEnable(Click& click) final
		{
			registerAsyncTickEventHandler(click, [this]
			{
				if (!m_on)
				{
					ManagedTunables::orbital_cannon_cooldown.reset();
					return false;
				}
				ManagedTunables::orbital_cannon_cooldown.set(0);
				return true;
			});
		}
	};
}
