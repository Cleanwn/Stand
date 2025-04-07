#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandLockGameplayCamHeading : public CommandToggle
	{
	public:
		explicit CommandLockGameplayCamHeading(CommandList* const parent)
			: CommandToggle(parent, LOC("CAMPUKE"), CMDNAMES("lockheading", "lockgameplayheading", "lockgameplaycamheading", "lockgameplaycameraheading"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(360.0f);
			});
		}
	};
}
