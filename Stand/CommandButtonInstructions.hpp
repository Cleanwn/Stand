#pragma once

#include "CommandListSelect.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandButtonInstructions : public CommandListSelect
	{
	public:
		explicit CommandButtonInstructions(CommandList* const parent)
			: CommandListSelect(parent, LOC("SHWBI"), { CMDNAME("showbuttoninstructions") }, NOLABEL, {
				{(long long)(ButtonInstructionsMode::ALWAYS), LOC("ON")},
				{(long long)(ButtonInstructionsMode::SMART), LOC("SMRT")},
				{(long long)(ButtonInstructionsMode::NEVER), LOC("DOFF")},
			}, (long long)(ButtonInstructionsMode::ALWAYS))
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_gui.button_instructions_mode = (ButtonInstructionsMode)value;
			}
		}
	};
}
