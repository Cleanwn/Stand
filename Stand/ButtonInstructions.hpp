#pragma once

#include "hashtype.hpp"

namespace Stand
{
	class ButtonInstructions
	{
	public:
		using Button_t = uint16_t;

		enum Button : Button_t
		{
			CLOSEMENU4MOREMOVE	= 0b1,
			YES_NO				= 0b10,
			ABORT_PROCEED		= 0b100,
			COMMANDBOX_CONTEXT	= 0b1000,
			LAND				= 0b10000,
			FREE_SPRINT			= 0b100000,
			ASC_DESC			= 0b1000000,
			OBJECT_ROTATE		= 0b10000000,
			FOCUS_IN_MENU		= 0b100000000,
			SUPER_FLIGHT		= 0b1000000000,
		};

		inline static Button_t gui_buttons = 0;
		inline static Button_t feature_buttons = 0;

		static void setFreeMovementThisTick();
		static void onTick();
	private:
		inline static int prev_buttons = 0;
		inline static uhash_t prev_scheme_hash = 0;
	};
}
