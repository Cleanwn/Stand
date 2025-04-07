#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandSelfFreeze : public CommandToggle
	{
	public:
		explicit CommandSelfFreeze(CommandList* const parent)
			: CommandToggle(parent, LOC("FRZ"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					ENTITY::FREEZE_ENTITY_POSITION(g_player_ped, TRUE);
					return true;
				}
				ENTITY::FREEZE_ENTITY_POSITION(g_player_ped, FALSE);
				return false;
			});
		}
	};
}
