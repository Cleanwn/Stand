#pragma once

#include "CommandPosition2d.hpp"

namespace Stand
{
	class CommandTypingPosOpen : public CommandPosition2d
	{
	private:
		bool was_active = false;

	public:
		explicit CommandTypingPosOpen(CommandList* parent)
			: CommandPosition2d(parent, LOC("TPNGPOS_O"), CMDNAME("opentyping"), { 1217, 1025 })
		{
		}

		void onChange(Position2d&& pos) const final
		{
			CommandShowTyping::pos_open = std::move(pos);
		}

		void onActiveListUpdate() final
		{
			if (isCurrentUiOrWebList())
			{
				if (!was_active)
				{
					was_active = true;
					CommandShowTyping::preview = CommandShowTyping::OPEN;
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
					if (CommandShowTyping::preview == CommandShowTyping::OPEN)
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
