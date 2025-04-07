#pragma once

#include "CommandSlider.hpp"

namespace Stand
{
	class CommandHairHighlight : public CommandSlider
	{
	public:
		explicit CommandHairHighlight(CommandList* const parent);

		void onChange(Click& click, int prev_value) final;

		void applyDefaultState() final
		{
		}
	};
}
