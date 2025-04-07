#pragma once

#include "CommandListAction.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandLevitationPresets : public CommandListAction
	{
	public:
		explicit CommandLevitationPresets(CommandList* const parent)
			: CommandListAction(parent, LOC("PRSTS"), {}, NOLABEL, {
				{ 0, LOC("LEV") },
				{ 1, LOC("NOCLIP") },
			})
		{
		}

		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(click, item);

			Click click_ = click.deriveAuto();
			g_gui.triggerCommands(L"levitate on", click_, parent);
			switch (item->value)
			{
			case 0:
				g_gui.triggerCommands(L"levitatespeed default", click_, parent);
				g_gui.triggerCommands(L"levitatesprintmultiplier default", click_, parent);
				g_gui.triggerCommands(L"levitateignorepitch default", click_, parent);
				g_gui.triggerCommands(L"levitatepassivemin default", click_, parent);
				g_gui.triggerCommands(L"levitatepassivemax default", click_, parent);
				g_gui.triggerCommands(L"levitatepassivespeed default", click_, parent);
				g_gui.triggerCommands(L"levitateassistup default", click_, parent);
				g_gui.triggerCommands(L"levitateassistdown default", click_, parent);
				g_gui.triggerCommands(L"levitateassistdeadzone default", click_, parent);
				g_gui.triggerCommands(L"levitateassistsnap default", click_, parent);
				break;

			case 1:
				g_gui.triggerCommands(L"levitatespeed 2.00", click_, parent);
				g_gui.triggerCommands(L"levitatesprintmultiplier 0.50", click_, parent);
				g_gui.triggerCommands(L"levitateignorepitch off", click_, parent);
				g_gui.triggerCommands(L"levitatepassivemin 0", click_, parent);
				g_gui.triggerCommands(L"levitatepassivemax 0", click_, parent);
				g_gui.triggerCommands(L"levitatepassivespeed 0", click_, parent);
				g_gui.triggerCommands(L"levitateassistup 0", click_, parent);
				g_gui.triggerCommands(L"levitateassistdown 0", click_, parent);
				g_gui.triggerCommands(L"levitateassistdeadzone 0", click_, parent);
				g_gui.triggerCommands(L"levitateassistsnap 0", click_, parent);
				break;
			}
		}
	};
}
