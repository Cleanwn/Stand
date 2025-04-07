#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandShowTransitionState : public CommandToggle
	{
	public:
		explicit CommandShowTransitionState(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;

	private:
		static void onPresent();

		static const wchar_t* transitionStateToString(const int transition_state);
	};
}
