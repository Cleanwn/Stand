#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandThousandsSep : public CommandToggle
	{
	public:
		explicit CommandThousandsSep(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;

		static void enablePatch();
		static void disablePatch();
	};
}
