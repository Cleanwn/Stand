#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandCustomGliderSetspeed : public CommandAction
	{
	public:
		explicit CommandCustomGliderSetspeed(CommandList* const parent)
			: CommandAction(parent, LOC("MAXPOS2SPD"))
		{
		}

		void onClick(Click& click) final
		{
			parent->as<CommandCustomGlider>()->speed->setValue(click, parent->as<CommandCustomGlider>()->max->value / 3);
		}
	};
}
