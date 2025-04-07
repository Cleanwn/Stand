#pragma once

#include "CommandToggle.hpp"

#include "pointers.hpp"

namespace Stand
{
	class CommandStutterFix : public CommandToggle
	{
	public:
		explicit CommandStutterFix(CommandList* const parent)
			: CommandToggle(parent, LOC("STUTFIX"), CMDNAMES("stutterfix", "fixstutters"), LOC("STUTFIX_H"))
		{
		}

		void onChange(Click& click) final
		{
			*pointers::rage__audRequestedSettings__sm_IgnoreBlockedAudioThread = m_on;
		}
	};
}
