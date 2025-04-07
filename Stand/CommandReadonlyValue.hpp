#pragma once

#include "CommandPhysical.hpp"

namespace Stand
{
	class CommandReadonlyValue : public CommandPhysical
	{
	public:
		std::wstring value;

		explicit CommandReadonlyValue(CommandList* parent, Label&& menu_name, Label&& help_text = NOLABEL, commandflags_t flags = CMDFLAGS_READONLY_VALUE, std::wstring&& value = {});

		void setValue(const std::string& value);
		void setValue(std::wstring&& value);
	protected:
		void updateWebState() const;
	};
}
