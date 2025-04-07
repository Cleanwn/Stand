#pragma once

#include "CommandColour.hpp"

namespace Stand
{
	class CommandNotifyBgColour : public CommandColour
	{
	public:
		explicit CommandNotifyBgColour(CommandList* parent, CommandColourCustom* with_copy_from)
			: CommandColour(parent, LOC("CLRBR"), { CMDNAME("notifybg") }, NOLABEL, 0, 0, 0, 77, CMDFLAGS_COLOUR, with_copy_from)
		{
		}

		void onChange(Click& click) final
		{
			g_renderer.notifyBgColour = getRGBA();
		}
	};
}
