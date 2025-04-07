#pragma once

#include "CommandReadonlyValue.hpp"

#include "StringCastable.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandReadonlyValueCopy : public CommandReadonlyValue
	{
	public:
		explicit CommandReadonlyValueCopy(CommandList* const parent, Label&& menu_name, const StringCastable& value, const commandflags_t flags = CMDFLAGS_READONLY_VALUE_COPY);
		explicit CommandReadonlyValueCopy(CommandList* const parent, Label&& menu_name, const std::string& value, const commandflags_t flags = CMDFLAGS_READONLY_VALUE_COPY);
		explicit CommandReadonlyValueCopy(CommandList* const parent, Label&& menu_name, std::wstring&& value = {}, const commandflags_t flags = CMDFLAGS_READONLY_VALUE_COPY);

		void onClick(Click& click) override;
	};
#pragma pack(pop)
}
