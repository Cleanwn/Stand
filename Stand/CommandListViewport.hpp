#pragma once

#include "CommandListWithOnTickAsActiveList.hpp"

#include "Gui.hpp"

namespace Stand
{
	template <typename T>
	class CommandListViewport : public CommandListWithOnTickAsActiveList<T>
	{
	public:
		explicit CommandListViewport(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL)
			: CommandListWithOnTickAsActiveList<T>(parent, std::move(menu_name), std::move(command_names), std::move(help_text), CMDFLAGS_LIST | CMDFLAG_CUSTOM_ON_TICK_IN_VIEWPORT)
		{
		}

		void onTickAsActiveList() override
		{
			if (g_gui.opened)
			{
				this->forEachPhysicalChild([](CommandPhysical* cmd)
				{
					// onTickInGameViewport is usually not dispatched for concealed commands, but CommandVehmodInt relies on it,
					// which is fine since they're always children of CommandListViewport.
					// If this `if` is ever uncommented, the isConcealed check in CommandWeaponComponent can be removed.
					//if (!cmd->isConcealed())
					{
						cmd->onTickInGameViewport();
					}
				});
			}
		}
	};
}
