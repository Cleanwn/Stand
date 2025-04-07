#pragma once

#include "CommandSlider.hpp"
#include "CommandSliderFloat.hpp"

namespace Stand
{
	template <class CommandSliderType>
	class CommandSliderNamedValue : public CommandSliderType
	{
	private:
		int value_to_name;
		std::wstring name_for_value;

	public:
		explicit CommandSliderNamedValue(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int value_to_name, std::wstring&& name_for_value, int min_value, int max_value, int default_value, int step_size = 1)
			: CommandSliderType(parent, std::move(menu_name), std::move(command_names), std::move(help_text), min_value, max_value, default_value, step_size), value_to_name(value_to_name), name_for_value(name_for_value)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (num == value_to_name && allow_replacements)
			{
				return name_for_value;
			}

			return CommandSliderType::formatNumber(num, allow_replacements);
		}
	};
}