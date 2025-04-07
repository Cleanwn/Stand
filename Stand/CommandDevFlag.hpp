#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDevFlag : public CommandToggle
	{
	public:
		explicit CommandDevFlag(CommandList* parent);

		void onChange(Click& click) final;
	};
}
