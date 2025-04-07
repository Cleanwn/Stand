#include "CommandSliderRainbow.hpp"

#include "get_current_time_millis.hpp"

namespace Stand
{
	CommandSliderRainbow::CommandSliderRainbow(CommandList* const parent, CommandSlider* slider, Label&& menu_name, std::vector<CommandName>&& command_names)
		: CommandSlider(parent, std::move(menu_name), std::move(command_names), NOLABEL, 0, 1000, 0), slider(slider)
	{
	}

	void CommandSliderRainbow::onChange(Click& click, int prev_value)
	{
		if (value > 0)
		{
			last_tick = get_current_time_millis();
			registerScriptTickEventHandler(click, [this]
			{
				if (value > 0)
				{
					const time_t now = get_current_time_millis();
					ms_passed += now - last_tick;
					last_tick = now;
					while (ms_passed >= value)
					{
						ms_passed -= value;
						Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
						if (slider->value >= slider->max_value)
						{
							slider->setValue(click, 0);
						}
						else
						{
							slider->setValue(click, slider->value + 1);
						}
					}
					return true;
				}
				return false;
			});
		}
	}
}
