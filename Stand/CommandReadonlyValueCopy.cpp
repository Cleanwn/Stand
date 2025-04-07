#include "CommandReadonlyValueCopy.hpp"

#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandReadonlyValueCopy::CommandReadonlyValueCopy(CommandList* const parent, Label&& menu_name, const StringCastable& value, const commandflags_t flags)
		: CommandReadonlyValueCopy(parent, std::move(menu_name), value.value, flags)
	{
	}

	CommandReadonlyValueCopy::CommandReadonlyValueCopy(CommandList* const parent, Label&& menu_name, const std::string& value, const commandflags_t flags)
		: CommandReadonlyValueCopy(parent, std::move(menu_name), StringUtils::utf8_to_utf16(value), flags)
	{
	}

	CommandReadonlyValueCopy::CommandReadonlyValueCopy(CommandList* const parent, Label&& menu_name, std::wstring&& value, const commandflags_t flags)
		: CommandReadonlyValue(parent, std::move(menu_name), LOC("C2C"), flags, std::move(value))
	{
	}

	void CommandReadonlyValueCopy::onClick(Click& click)
    {
		Util::copy_to_clipboard_utf16(click, value);
    }
}
