#pragma once

#include "CommandListWarning.hpp"

namespace Stand
{
	class CommandListProtections : public CommandListWarning
	{
	public:
		explicit CommandListProtections(CommandList* const parent);

	protected:
		bool shouldShowWarning() const final;
	};
}
