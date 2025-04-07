#pragma once

#include "CommandColour.hpp"

namespace Stand
{
	class CommandNotifyFlashColour : public CommandColour
	{
	public:
		explicit CommandNotifyFlashColour(CommandList* parent)
			: CommandColour(parent, LOC("FLSHCLR"), { CMDNAME("notifyflash") }, NOLABEL, { 0.6196f, 0.0f, 0.6196f, 1.0f })
		{
		}

		void onChange(Click& click) final
		{
			g_renderer.notifyFlashColour = getRGBA();
		}
	};
}
