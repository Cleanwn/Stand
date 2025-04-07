#pragma once

#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandVisualZCorrection : public CommandSliderFloat
	{
	private:
		UniqueThread* thread = nullptr;
		time_t fall_start = 0;
		time_t timeout_end = 0;

	public:
		explicit CommandVisualZCorrection(CommandList* parent);

		void onChange(Click& click, int prev_value) final;
	private:
		void stopThreadIfRunning();
		void ensureThreadIsRunning();
	};
}
