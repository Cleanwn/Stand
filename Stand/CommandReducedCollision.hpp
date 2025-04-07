#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandGhost : public CommandToggle
	{
	public:
		explicit CommandGhost(CommandList* const parent)
			: CommandToggle(parent, LOC("GHOST"), { CMDNAME("reducedcollision"), CMDNAME("ghostmode") }, LOC("GHOST_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []()
			{
				PED::SET_PED_CAPSULE(g_player_ped, 1.175494e-38f);
			});
		}
	};
}
