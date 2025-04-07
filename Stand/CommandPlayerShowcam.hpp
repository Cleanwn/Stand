#pragma once

#include "CommandPlayerToggleBatch.hpp"

namespace Stand
{
	class CommandPlayerShowcam : public CommandPlayerToggleBatch
	{
	public:
		explicit CommandPlayerShowcam(CommandList* const parent);

		void onTick(const std::vector<AbstractPlayer>& players) const final;
	};
}
