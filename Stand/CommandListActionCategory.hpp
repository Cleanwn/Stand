#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListActionCategory : public CommandList
	{
	public:
		explicit CommandListActionCategory(CommandList* const parent, Label menu_name)
			: CommandList(parent, std::move(menu_name))
		{
		}

		void onClick(Click& click) final;
	};
}
