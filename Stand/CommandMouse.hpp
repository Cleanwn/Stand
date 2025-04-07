#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandMouse : public CommandToggle
	{
	public:
		explicit CommandMouse(CommandList* const parent)
			: CommandToggle(parent, LOC("MOUSE"), CMDNAMES("mouse"), LOC("MOUSE_H"))
		{
		}

		void onEnable(Click& click) final
		{
			if (click.isAuto())
			{
				g_gui.setMouseMode(MouseMode::NAVIGATE);
			}
			else
			{
				g_gui.doPromptUserForDisablePreset(click.thread_context, ATSTRINGHASH("JUSTMOUSE"));
			}
		}

		void onDisable(Click& click) final
		{
			g_gui.setMouseMode(MouseMode::NONE);
		}
	};
}
