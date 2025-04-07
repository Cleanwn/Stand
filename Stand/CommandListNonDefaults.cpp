#include "CommandListNonDefaults.hpp"

#include "CommandLink.hpp"
#include "Gui.hpp"

namespace Stand
{
	CommandListNonDefaults::CommandListNonDefaults(CommandList* parent)
		: CommandList(parent, LOC("LNDFT"))
	{
	}

	void CommandListNonDefaults::onActiveListUpdate()
	{
		if (isCurrentMenuOrWebList())
		{
			if (!active)
			{
				active = true;
				resetChildren();
				g_gui.root_list->recursiveForEachChild([this](const std::unique_ptr<Command>& cmd)
				{
					if (cmd->isPhysical()
						&& cmd->as<CommandPhysical>()->supportsStateOperations()
						&& !(cmd->as<CommandPhysical>()->flags & CMDFLAG_TEMPORARY)
						&& cmd->as<CommandPhysical>()->getState() != cmd->as<CommandPhysical>()->getDefaultState()
						)
					{
						createChild<CommandLink>(cmd.get(), true);
						return false;
					}
					return true;
				});
			}
		}
		else
		{
			active = false;
		}
	}
}
