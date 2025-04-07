#include "CommandMmRegion.hpp"

#include "CommandMmRegionToggle.hpp"
#include "CommandMmRegionSelect.hpp"

namespace Stand
{
	CommandMmRegion::CommandMmRegion(CommandList* const parent)
		: CommandList(parent, LOC("MMREGION2"))
	{
		toggle = createChild<CommandMmRegionToggle>();
		select = createChild<CommandMmRegionSelect>();
	}
}
