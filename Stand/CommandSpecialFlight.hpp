#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandSpecialFlight : public CommandListSelect
	{
	private:
		bool transitioning = false;

	public:
		explicit CommandSpecialFlight(CommandList* const parent);

		void onChange(Click& click, long long prev_value) final;
	private:
		static void cleanupHandlingData();
	};
}
