#pragma once

#include "CommandToggleCustom.hpp"

namespace Stand
{
	class CommandProfileAutosave : public CommandToggleCustom
	{
	public:
		explicit CommandProfileAutosave(CommandList* const parent)
			: CommandToggleCustom(parent, LOC("AUTOSAVE2"), { CMDNAME("autosave"), CMDNAME("autosaveprofile") }, LOC("AUTOSAVE2_H"), CMDFLAGS_TOGGLE_CUSTOM | CMDFLAG_TEMPORARY)
		{
			addSuffixToCommandNames(parent->menu_name.getLiteralForCommandName());
		}

		void onEnable(Click& click) final
		{
			g_gui.enableAutosave(parent->menu_name.getLiteralUtf16());
			g_gui.meta_state.data.emplace("Autosave", parent->as<CommandProfile>()->getProfileNameA());
			g_gui.meta_state.save();
		}

		void onDisable(Click& click) final
		{
			if (click.type != CLICK_BULK)
			{
				if (g_gui.command_state_change_save.has_value()) // Autosave pending?
				{
					g_gui.saveState(std::wstring(g_gui.auto_save_state));
				}
				g_gui.disableAutosave();
				g_gui.meta_state.data.erase("Autosave");
				g_gui.meta_state.save();
			}
		}

		void onTickInGameViewport() final
		{
			const bool on = (g_gui.auto_save_state == parent->as<CommandProfile>()->getConfigName());
			bool temp_off = false;
			if (!on)
			{
				auto e = g_gui.meta_state.data.find("Autosave");
				if (e != g_gui.meta_state.data.end())
				{
					if (e->second == parent->as<CommandProfile>()->getProfileNameA())
					{
						temp_off = true;
					}
				}
			}

			setIndicators(on, temp_off);
			if (temp_off)
			{
				setHelpText(LOC("AUTOSAVE2_H_T"));
			}
			else
			{
				setHelpText(LOC("AUTOSAVE2_H"));
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
