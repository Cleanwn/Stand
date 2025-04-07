#pragma once

#include "CommandList.hpp"

#include "CommandLockWeaponsSavePreset.hpp"
#include "CommandRedirectToList.hpp"
#include "CommandScripts.hpp"
#include "get_current_time_millis.hpp"

#include "Util.hpp"

namespace Stand
{
	class CommandLockWeaponsPresets : public CommandList
	{
	private:
		CommandPhysical* redirect = nullptr;

	public:
		explicit CommandLockWeaponsPresets(CommandList* const parent)
			: CommandList(parent, LOC("PRSTS"), CMDNAMES_OBF("lockweaponspresets"))
		{
			createChild<CommandLockWeaponsSavePreset>();
		}

		void refreshScriptsLink()
		{
			CommandScripts::instance->refresh([this]
			{
				CommandList* scripts_folder = CommandScripts::instance->findFolder("Weapon Loadouts");
				if (scripts_folder == nullptr)
				{
					if (redirect != nullptr)
					{
						redirect->beGone();
						redirect = nullptr;
					}
				}
				else
				{
					if (redirect == nullptr)
					{
						auto link = this->createChild<CommandRedirectToList>(scripts_folder, LOC("VWPRSTS"));
						redirect = link;
						processChildrenUpdate();
					}
				}
			});
		}

		void onActiveListUpdate()
		{
			refreshScriptsLink();

			return CommandList::onActiveListUpdate();
		}
	};
}