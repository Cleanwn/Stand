#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandCustomGlider : public CommandList
	{
	public:
		CommandSlider* min;
		CommandSlider* max;
		CommandSlider* speed;

		explicit CommandCustomGlider(CommandList* const parent);
	};
}
