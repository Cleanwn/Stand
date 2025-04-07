#pragma once

#include "CommandSlider.hpp"

#include "tbCustomFov.hpp"

namespace Stand
{
	class CommandFov : public CommandSlider
	{
	public:
		explicit CommandFov(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names)
			: CommandSlider(parent, std::move(menu_name), std::move(command_names), NOLABEL, -5, 360, -5, 5)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (allow_replacements)
			{
				if (num < 0)
				{
					return LANG_GET_W("OVRDOFF");
				}
			}
			return CommandSlider::formatNumber(num);
		}

		void onChange(Click& click, int prev_value) final
		{
			if (value < 0)
			{
				if (prev_value > 0)
				{
					g_tb_custom_fov.disable();
				}
			}
			else
			{
				if (prev_value < 0)
				{
					g_tb_custom_fov.enable();
				}
			}
		}
	};
}
