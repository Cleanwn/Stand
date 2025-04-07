#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandNoCollision : public CommandToggle
	{
	public:
		explicit CommandNoCollision(CommandList* const parent)
			: CommandToggle(parent, LOC("GHOSTRIDE"), { CMDNAME("nocollision") })
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(g_player_ped, false, false);
					return true;
				}
				ENTITY::SET_ENTITY_COLLISION(g_player_ped, true, false);
				return false;
			});
		}
	};
}
