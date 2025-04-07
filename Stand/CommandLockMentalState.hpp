#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandLockMentalState : public CommandToggle
	{
	public:
		explicit CommandLockMentalState(CommandList* const parent)
			: CommandToggle(parent, LOC("LCKMNTL"), CMDNAMES_OBF("lockmentalstate"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(this->m_on);
				if (is_session_started_and_transition_finished())
				{
					if (const auto slot = Util::get_char_slot_no_notify(); slot != -1)
					{
						const auto fvalue = (float)parent->as<CommandListMental>()->mental->value;
						if (*g_player.mentalState() != fvalue)
						{
							if (!parent->as<CommandListMental>()->pulse->m_on)
							{
								*g_player.mentalState() = fvalue;
							}
							STATS::STAT_SET_FLOAT(CHAR_STAT(slot, "PLAYER_MENTAL_STATE"), fvalue, true);
						}
					}
				}
				HANDLER_END;
			});
		}
	};
}