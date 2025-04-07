#include "CommandHairHighlight.hpp"

#include "CommandLockOutfit.hpp"

namespace Stand
{
	CommandHairHighlight::CommandHairHighlight(CommandList* const parent)
		: CommandSlider(parent, LOC("HAIRHI"), CMDNAMES("hairhighlight", "hairlight"), NOLABEL, -1, 64, -1, 1, (CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_FEATURELIST_FINISHLIST)
	{
	}

	void CommandHairHighlight::onChange(Click& click, int prev_value)
	{
		ensureScriptThread(click, [this]
		{
			CommandLockOutfit::setHairColourFromCommands();
		});
	}
}
