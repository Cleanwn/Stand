#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandRespawnRecall : public CommandToggle
	{
	public:
		v3 pos;

		explicit CommandRespawnRecall(CommandList* parent)
			: CommandToggle(parent, LOC("RESPWRCL"), CMDNAMES("respawnrecall"))
		{
		}

		void onEnable(Click& click) final
		{
			pos.x = -FLT_MAX;
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					if (pos.x == -FLT_MAX)
					{
						if (g_player_ped.isDead())
						{
							pos = g_player_ped.getPos();
						}
					}
					else
					{
						if (!g_player_ped.isDead()
							&& (CAM::IS_SCREEN_FADED_IN() || is_session_started())
							)
						{
							g_player_ped.setPos(pos);
							pos.x = -FLT_MAX;
						}
					}
					return true;
				}
				return false;
			});
		}
	};
}
