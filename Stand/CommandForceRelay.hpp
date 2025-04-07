#pragma once

#include "CommandToggle.hpp"

#include <soup/Bytepatch.hpp>

namespace Stand
{
	class CommandForceRelay : public CommandToggle
	{
	private:
		soup::Bytepatch patch;
		
	public:
		explicit CommandForceRelay(CommandList* const parent);

		void onEnable(Click& click);
		void onDisable(Click& click);
	};
}
