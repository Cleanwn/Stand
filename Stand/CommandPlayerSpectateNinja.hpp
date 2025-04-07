#pragma once

#include "CommandPlayerSpectate.hpp"

namespace Stand
{
	class CommandPlayerSpectateNinja : public CommandPlayerSpectate
	{
	public:
		explicit CommandPlayerSpectateNinja(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
