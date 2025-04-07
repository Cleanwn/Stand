#pragma once

#include "CommandPositionSelection.hpp"

namespace Stand
{
	class CommandSpoofedPosition : public CommandPositionSelection
	{
	public:
		explicit CommandSpoofedPosition(CommandList* const parent)
			: CommandPositionSelection(parent, LOC("SPFDPOS"), CMDNAMES_OBF("spoofedposition"))
		{
		}

	protected:
		void onChange() final
		{
			g_comp_spoofpos.spoofed_position = pos;
		}
	};
}
