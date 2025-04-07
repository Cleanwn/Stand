#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandDisableFlightMusic : public CommandToggle
	{
	public:
		explicit CommandDisableFlightMusic(CommandList* parent)
			: CommandToggle(parent, LOC("NFLTMSC"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("flightmusic")))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				AUDIO::SET_AUDIO_FLAG("DisableFlightMusic", m_on);
				return m_on;
			});
		}
	};
}
