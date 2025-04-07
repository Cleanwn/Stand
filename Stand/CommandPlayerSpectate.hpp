#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerSpectate : public CommandPlayerToggle
	{
	public:
		inline static bool dedup = false;

		using CommandPlayerToggle::CommandPlayerToggle;

		Label getActivationName() const final
		{
			return getActivationNameImplCombineWithParent(": ");
		}
	};
}
