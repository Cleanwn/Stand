#pragma once

#include "CommandRainbow.hpp"

namespace Stand
{
	class CommandMenuRainbow : public CommandRainbow
	{
	private:
		CommandColourCustom* const focusTextColour;
		CommandColourCustom* const focusRightTextColour;
		CommandColourCustom* const focusSpriteColour;

	public:
		explicit CommandMenuRainbow(CommandList* const parent, CommandColourCustom* primaryColour, CommandColourCustom* focusTextColour, CommandColourCustom* focusRightTextColour, CommandColourCustom* focusSpriteColour)
			: CommandRainbow(parent, LOC("LGBT"), { CMDNAME("rainbow") }, primaryColour, onHueChange), focusTextColour(focusTextColour), focusRightTextColour(focusRightTextColour), focusSpriteColour(focusSpriteColour)
		{
		}

	private:
		static void onHueChange(CommandRainbow* _thisptr);
	};
}
