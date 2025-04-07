#pragma once

#include "CommandToggleNoCorrelation.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandToggleCustom : public CommandToggleNoCorrelation
	{
	public:
		bool auto_indicator = false;

		explicit CommandToggleCustom(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const commandflags_t flags = CMDFLAGS_TOGGLE_CUSTOM)
			: CommandToggleNoCorrelation(parent, std::move(menu_name), std::move(command_names), std::move(help_text), false, flags, COMMANDPERM_USERONLY, {}, COMMAND_TOGGLE_CUSTOM)
		{
		}

		void setIndicators(bool on, bool auto_indicator)
		{
			if (m_on != on || this->auto_indicator != auto_indicator)
			{
				m_on = on;
				this->auto_indicator = auto_indicator;
				processVisualUpdate(true);
				updateWebState();
			}
		}
	};
#pragma pack(pop)
}
