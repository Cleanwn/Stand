#pragma once

#include "CommandPhysical.hpp"

namespace Stand
{
	class CommandReadonlyName : public CommandPhysical
	{
	public:
		explicit CommandReadonlyName(CommandList* const parent, Label&& menu_name, Label&& help_text = NOLABEL);

		void onClick(Click& click) final;
	};
}
