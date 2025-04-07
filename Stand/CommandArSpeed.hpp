#pragma once

#include "CommandToggle.hpp"

#include "CommandColour.hpp"

namespace Stand
{
	class CommandArSpeed : public CommandToggle
	{
	private:
		CommandColour* const colour;
		CommandToggleNoCorrelation* const alt_rendering;

	public:
		explicit CommandArSpeed(CommandList* const parent, CommandColour* const colour, CommandToggleNoCorrelation* const alt_rendering);

		void onEnable(Click& click) final;
	};
}
