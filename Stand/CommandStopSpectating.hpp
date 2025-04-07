#pragma once

#include "CommandAction.hpp"

#include "CommandPlayerSpectate.hpp"

namespace Stand
{
	class CommandStopSpectating : public CommandAction
	{
	public:
		explicit CommandStopSpectating(CommandList* const parent)
			: CommandAction(parent, LOC("STPSPEC"), CMDNAMES_OBF("stopspectating", "unspectate"))
		{
		}

		void onClick(Click& click) final
		{
			CommandPlayerSpectate::dedup = false;
		}
	};
}
