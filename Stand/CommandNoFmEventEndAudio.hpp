#pragma once

#include "CommandToggle.hpp"

#include "is_session.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandNoFmEventEndAudio : public CommandToggle
	{
	public:
		explicit CommandNoFmEventEndAudio(CommandList* const parent)
			: CommandToggle(parent, LOC("NFMEVTENDAUD"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAMES("eventendaudio")), LOC("NOFMBTLMSC_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (is_session_started())
				{
					// IF !IS_BIT_SET(MPGlobalsAmbience.iBS_AmbientEventAudioTriggers, ciAE_AUDIO_PLAY_COUNTDOWN)
					*ScriptGlobal(GLOBAL_EVENT_AUDIO_TRIGGERS).as<int*>() |= (1 << 1);
				}
			});
			if (!m_on)
			{
				ensureScriptThread(click, []
				{
					*ScriptGlobal(GLOBAL_EVENT_AUDIO_TRIGGERS).as<int*>() &= ~(1 << 1);
				});
			}
		}
	};
}
