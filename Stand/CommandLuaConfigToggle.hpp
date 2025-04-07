#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandLuaConfigToggle : public CommandToggle
	{
	public:
		using CommandToggle::CommandToggle;

		void onChange(Click& click) final
		{
			if (click.type == CLICK_SCRIPTED)
			{
				return onChangeImplUnavailable(click);
			}
			return CommandToggle::onChange(click);
		}
	};
}
