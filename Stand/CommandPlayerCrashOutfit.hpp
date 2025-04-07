#pragma once

#include "CommandPlayerCrash.hpp"

namespace Stand
{
	class CommandPlayerCrashOutfit : public CommandPlayerCrash
	{
	public:
		explicit CommandPlayerCrashOutfit(CommandList* const parent);

		void expandDong(std::vector<AbstractPlayer>&& targets) const final;
	};
}
