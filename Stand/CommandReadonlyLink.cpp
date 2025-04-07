#include "CommandReadonlyLink.hpp"

#include <fmt/core.h>

#include "Gui.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandReadonlyLink::CommandReadonlyLink(CommandList* const parent, Label&& menu_name, std::string&& link, Label&& help_text, std::vector<CommandName>&& command_names)
		: CommandPhysical(COMMAND_READONLY_LINK, parent, std::move(menu_name), std::move(command_names), std::move(help_text)), link(std::move(link))
	{
	}

	void CommandReadonlyLink::onClick(Click& click)
	{
		g_gui.shellExecute(link.c_str());
	}
}
