#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerCeopay : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerCeopay(CommandList* parent);

		void onClick(Click& click) final;

		void onEnable(Click& click) final;
	};
}
