#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandShowArraySynchProgress : public CommandToggle
	{
	public:
		explicit CommandShowArraySynchProgress(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;

	private:
		static void onTick();
		static void onPresent();
	};
}
