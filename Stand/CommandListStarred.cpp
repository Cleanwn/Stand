#include "CommandListStarred.hpp"

#include "CommandLink.hpp"
#include "Gui.hpp"

namespace Stand
{
	CommandListStarred::CommandListStarred(CommandList* parent)
		: CommandList(parent, LOC("CMDSTARS"), {}, LOC("CMDSTARS_H"))
	{
	}

	void CommandListStarred::onActiveListUpdate()
	{
		if (isCurrentUiOrWebList())
		{
			resetChildren();
			recursivelyPopulate(g_gui.root_list.get(), {});
			processChildrenUpdate();
		}
	}

	void CommandListStarred::recursivelyPopulate(CommandList* list, const std::string& path_prefix)
	{
		for (auto& child : list->children)
		{
			if (!child->isPhysical())
			{
				continue;
			}
			auto* cmd = (CommandPhysical*)child.get();
			if (cmd->flags & CMDFLAG_STARRED_FINISHLIST)
			{
				break;
			}
			std::string path = std::move(std::string(path_prefix).append(cmd->getNameForConfig()));
			if (!(cmd->flags & CMDFLAG_TEMPORARY))
			{
				auto i = g_gui.starred_commands.data.find(path);
				if (i != g_gui.starred_commands.data.end() && i->second == "Saved")
				{
					this->createChild<CommandLink>(cmd, true);
				}
			}
			if (cmd->isList())
			{
				path.push_back('>');

				for (const auto& entry : g_gui.starred_commands.data)
				{
					if (entry.first.length() > path.length()
						&& entry.first.substr(0, path.length()) == path
						)
					{
						recursivelyPopulate((CommandList*)cmd, path);
						break;
					}
				}
			}
		}
	}
}
