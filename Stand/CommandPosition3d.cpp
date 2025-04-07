#include "CommandPosition3d.hpp"

#include "CommandVector3Slider.hpp"
#include "CommandVector3Copy.hpp"

namespace Stand
{
	CommandPosition3d::CommandPosition3d(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names)
		: CommandVector3(parent, std::move(menu_name), std::move(command_names))
	{
	}

	void CommandPosition3d::populate()
	{
		createChild<CommandVector3Slider<&v3::x>>(LIT("X"), getSuffixedCommandNames(CMDNAME("x")));
		createChild<CommandVector3Slider<&v3::y>>(LIT("Y"), getSuffixedCommandNames(CMDNAME("y")));
		createChild<CommandVector3Slider<&v3::z>>(LOC("POS_Z"), getSuffixedCommandNames(CMDNAME("z")));
		createChild<CommandVector3Copy>(getPrefixedCommandNames(CMDNAME("copy")));
	}
}
