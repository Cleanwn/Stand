#pragma once

#include "CommandListAction.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandSuperDrivePresets : public CommandListAction
	{
	public:
		explicit CommandSuperDrivePresets(CommandList* const parent)
			: CommandListAction(parent, LOC("PRSTS"), {}, NOLABEL, {
				{ 0, LOC("SPRDRV") },
				{ 1, LOC("HORNBOOST") },
			})
		{
		}

		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(click, item);

			Click click_ = click.deriveAuto();
			g_gui.triggerCommands(L"superdrive on", click_, parent);
			switch (item->value)
			{
			case 0:
				g_gui.triggerCommands(L"hotkeyadd superdrive w", click_);
				g_gui.triggerCommands(L"hotkeyremove superdrive e", click_);
				g_gui.triggerCommands(L"superdrivecam default", click_, parent);
				g_gui.triggerCommands(L"superdrivespeed default", click_, parent);
				g_gui.triggerCommands(L"superdrivecontrollerbutton default", click_, parent);
				break;

			case 1:
				g_gui.triggerCommands(L"hotkeyadd superdrive e", click_);
				g_gui.triggerCommands(L"hotkeyremove superdrive w", click_);
				g_gui.triggerCommands(L"superdrivecam off", click_, parent);
				g_gui.triggerCommands(L"superdrivespeed 10.00", click_, parent);
				g_gui.triggerCommands(L"superdrivecontrollerbutton l3", click_, parent);
				break;
			}
		}
	};
}
