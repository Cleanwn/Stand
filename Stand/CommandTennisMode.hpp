#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandTennisMode : public CommandToggle
	{
	public:
		explicit CommandTennisMode(CommandList* parent)
			: CommandToggle(parent, LOC("TNISMDE"), CMDNAMES("tennismode"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if ((bool)MISC::IS_TENNIS_MODE(g_player_ped) != m_on)
				{
					MISC::ENABLE_TENNIS_MODE(g_player_ped, m_on, m_on);
				}
				return m_on;
			});
		}
	};
}
