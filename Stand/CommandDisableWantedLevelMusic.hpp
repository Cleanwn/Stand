#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDisableWantedLevelMusic : public CommandToggle
	{
	public:
		explicit CommandDisableWantedLevelMusic(CommandList* parent)
			: CommandToggle(parent, LOC("NWNTMSC"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("wantedlevelmusic")))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				AUDIO::SET_AUDIO_FLAG("WantedMusicDisabled", m_on);
				return m_on;
			});
		}
	};
}
