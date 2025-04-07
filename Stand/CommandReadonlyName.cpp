#include "CommandReadonlyName.hpp"

#include "lang.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandReadonlyName::CommandReadonlyName(CommandList* const parent, Label&& menu_name, Label&& help_text)
		: CommandPhysical(COMMAND_READONLY_NAME, parent, std::move(menu_name), {}, std::move(help_text))
	{
	}

	void CommandReadonlyName::onClick(Click& click)
	{
		Util::copy_to_clipboard(click, menu_name.getLocalisedUtf8());
	}
}
