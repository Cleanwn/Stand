#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandClubPopLock : public CommandToggle
	{
	public:
		explicit CommandClubPopLock(CommandList* const parent)
			: CommandToggle(parent, LOC("CLUBPOP_L"), CMDNAMES_OBF("locknightclubpopularity"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(this->m_on);
				if (is_session_started())
				{
					Click click(CLICK_AUTO);
					this->parent->children.at(0)->as<CommandClubPop>()->onClick(click);
				}
				HANDLER_END;
			});
		}
	};
}