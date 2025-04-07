#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandMentalPulse : public CommandToggle
	{
	public:
		int dir = +1;
		float val = 0.0f;

		explicit CommandMentalPulse(CommandList* const parent)
			: CommandToggle(parent, LOC("MNTLPULS"), {}, LOC("MNTLPULS_H"))
		{
		}

		void onEnable(Click& click) final
		{
			val = g_player.getMentalState();
			updateDirection();

			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);

				if (is_session_started_and_transition_finished())
				{
					*g_player.mentalState() = val;
				}

				val += (dir * 0.1f);
				updateDirection();

				HANDLER_END;
			});
		}

		void updateDirection() noexcept
		{
			if (val <= 0)
			{
				dir = +1;
			}
			else if (val >= 100)
			{
				dir = -1;
			}
		}
	};
}
