#pragma once

#include "CommandSlider.hpp"

namespace Stand
{
	class CommandSliderRainbow : public CommandSlider
	{
	private:
		CommandSlider* const slider;
		time_t ms_passed = 0;
		time_t last_tick = 0;

	public:
		explicit CommandSliderRainbow(CommandList* const parent, CommandSlider* slider, Label&& menu_name, std::vector<CommandName>&& command_names);

		void onChange(Click& click, int prev_value) final;
	};
}
