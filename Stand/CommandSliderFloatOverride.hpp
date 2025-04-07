#pragma once

#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandSliderFloatOverride : public CommandSliderFloat
	{
	private:
		int override_off_num;

	public:
		explicit CommandSliderFloatOverride(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const unsigned int step_size, int override_off_num)
			: CommandSliderFloat(parent, std::move(menu_name), std::move(command_names), std::move(help_text), min_value, max_value, default_value, step_size), override_off_num(override_off_num)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (allow_replacements)
			{
				if (num == override_off_num)
				{
					return LANG_GET_W("OVRDOFF");
				}
			}

			return CommandSliderFloat::formatNumber(num, allow_replacements);
		}
	};
}