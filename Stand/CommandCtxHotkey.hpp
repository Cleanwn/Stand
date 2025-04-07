#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandCtxHotkey : public CommandList
	{
	public:
		Hotkey* const hotkey;

		explicit CommandCtxHotkey(CommandList* parent, Hotkey* hotkey, CommandPhysical* target);

		void save(CommandPhysical* target);
	};
}
