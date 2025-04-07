#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandScaleCam : public CommandToggle
	{
	public:
		explicit CommandScaleCam(CommandList* const parent)
			: CommandToggle(parent, LOC("SCLECAM"), { CMDNAME("scalecam") }, NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			ensureScriptThread(click, []
			{
				g_gui.set_camera_distance_from_character_multiplier_all_things_considered();
			});
		}
	};
}
