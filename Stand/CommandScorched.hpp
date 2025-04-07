#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandScorched : public CommandToggle
	{
	public:
		explicit CommandScorched(CommandList* const parent);

		void onChange(Click& click) final;
	};
}
