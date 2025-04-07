#pragma once

#include "CommandTogglePointer.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandRevealLabels : public CommandTogglePointer
	{
	public:
		explicit CommandRevealLabels(CommandList* const parent)
			: CommandTogglePointer(parent, &g_hooking.reveal_labels, LOC("RVLLB"), CMDNAMES("reveallabels"), LOC("RVLLB_H"))
		{
		}
	};
}
