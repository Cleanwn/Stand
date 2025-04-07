#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandCustomsShowNa : public CommandToggle
	{
	public:
		inline static CommandCustomsShowNa* instance;

		explicit CommandCustomsShowNa(CommandList* const parent)
			: CommandToggle(parent, LOC("SHWNA"))
		{
			instance = this;
		}
	};
}
