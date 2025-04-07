#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandBlipTags : public CommandToggle
	{
	public:
		explicit CommandBlipTags(CommandList* const parent);

		void onChange(Click& click) final;
		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
