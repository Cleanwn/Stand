#pragma once

#include "CommandList.hpp"

#include "Animations.hpp"
#include "CommandAnimation.hpp"

namespace Stand
{
	class CommandScenarios : public CommandList
	{
	public:
		explicit CommandScenarios(CommandList* const parent)
			: CommandList(parent, LOC("SCENARIOS"), { CMDNAME("scenarios") })
		{
			for (auto& scenario : Animations::scenarios)
			{
				this->createChild<CommandAnimation<SimpleAnimation>>(scenario);
			}
		}
	};
}