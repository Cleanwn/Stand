#pragma once

#include "CommandToggle.hpp"

#include "is_session.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandRevealMap : public CommandToggle
	{
	public:
		explicit CommandRevealMap(CommandList* parent)
			: CommandToggle(parent, LOC("RVLMAP"), { CMDNAME("revealmap") })
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					HUD::SET_MINIMAP_HIDE_FOW(true);
					return true;
				}
				else
				{
					HUD::SET_MINIMAP_HIDE_FOW(is_session_started());
					return false;
				}
			});
		}
	};
}
