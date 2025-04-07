#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandOnlyRemoveSelectedWeapons : public CommandToggle
	{
	public:
		explicit CommandOnlyRemoveSelectedWeapons(CommandList* const parent)
			: CommandToggle(parent, LOC("WEAPLOCK_INV"), CMDNAMES_OBF("invertlockweapons"), LOC("WEAPLOCK_INV_H"))
		{
		}

		void onChange(Click& click) final
		{
			if (!g_gui.isUnloadPending()) // CommandToggleLockWeapons::onChange handles the unload case. Restoring twice during unload will apply an empty loadout.
			{
				ensureScriptThread(click, [this]
				{
					parent->children.at(0)->as<CommandToggleLockWeapons>()->restoreLoadout(false);				
				});
			}
		}
	};
}