#pragma once

#include "CommandToggle.hpp"

#include "TransitionHelper.hpp"

namespace Stand
{
	class CommandTransitionHelper : public CommandToggle
	{
	public:
		explicit CommandTransitionHelper(CommandList* const parent)
			: CommandToggle(parent, LOC("TRNSHLP"), CMDNAMES("transitionhelper"), LOC("TRNSHLP_H"), true)
		{
		}

		void onChange(Click& click) final
		{
			TransitionHelper::fix_stucks = m_on;
		}
	};
}
