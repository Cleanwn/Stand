#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandWaterwheels : public CommandToggle
	{
	public:
		explicit CommandWaterwheels(CommandList* const parent);

		void onChange(Click& click) final;
	};
}
