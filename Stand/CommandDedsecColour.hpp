#pragma once

#include "CommandColour.hpp"

#include "AllEntitiesEveryTick.hpp"

namespace Stand
{
	class CommandDedsecColour : public CommandColour
	{
	public:
		explicit CommandDedsecColour(CommandList* const parent)
			: CommandColour(parent, LOC("CLR"), { CMDNAME("dedsec") }, NOLABEL, 255, 0, 255, 255)
		{
		}

		void onChange(Click& click) final
		{
			AllEntitiesEveryTick::dedsec_r = r->value;
			AllEntitiesEveryTick::dedsec_g = g->value;
			AllEntitiesEveryTick::dedsec_b = b->value;
			AllEntitiesEveryTick::dedsec_a = a->value;
		}
	};
}
