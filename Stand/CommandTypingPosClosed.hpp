#pragma once

#include "CommandPosition2d.hpp"

namespace Stand
{
	class CommandTypingPosClosed : public CommandPosition2d
	{
	private:
		bool was_active = false;

	public:
		explicit CommandTypingPosClosed(CommandList* parent)
			: CommandPosition2d(parent, LOC("TPNGPOS_C"), CMDNAME("closedtyping"), { 1217, 1080 })
		{
		}

		void onChange(Position2d&& pos) const final
		{
			CommandShowTyping::pos_closed = std::move(pos);
		}

		void onActiveListUpdate() final
		{
			if (isCurrentUiOrWebList())
			{
				if (!was_active)
				{
					was_active = true;
					CommandShowTyping::preview = CommandShowTyping::CLOSED;
					if (!CommandShowTyping::instance->m_on)
					{
						CommandShowTyping::instance->enabled_for_preview = true;
						Click click(CLICK_AUTO);
						CommandShowTyping::instance->setStateBool(click, true);
					}
				}
			}
			else
			{
				if (was_active)
				{
					was_active = false;
					if (CommandShowTyping::preview == CommandShowTyping::CLOSED)
					{
						CommandShowTyping::preview = CommandShowTyping::NONE;
					}
					if (CommandShowTyping::instance->enabled_for_preview)
					{
						CommandShowTyping::instance->enabled_for_preview = false;
						Click click(CLICK_AUTO);
						CommandShowTyping::instance->setStateBool(click, false);
					}
				}
			}
		}
	};
}
