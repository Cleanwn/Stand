#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandRenderHdOnly : public CommandToggle
	{
	public:
		explicit CommandRenderHdOnly(CommandList* parent)
			: CommandToggle(parent, LOC("RDRHDOLY"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				STREAMING::SET_RENDER_HD_ONLY(m_on);
				return m_on;
			});
		}
	};
}
