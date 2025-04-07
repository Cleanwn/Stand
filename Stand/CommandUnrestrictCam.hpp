#pragma once

#include "CommandToggle.hpp"

#include "unrestrictGameplayCam.hpp"

namespace Stand
{
	class CommandUnrestrictCam : public CommandToggle
	{
	public:
		explicit CommandUnrestrictCam(CommandList* const parent)
			: CommandToggle(parent, LOC("UNRSTRCAM"), CMDNAMES("unrestrictcam"), LOC("UNRSTRCAM_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				unrestrictGameplayCam();
			});
		}
	};
}
