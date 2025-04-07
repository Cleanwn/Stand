#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDrunkMode : public CommandToggle
	{
	private:
		time_t drunk_steer_delay = 0;
		float steer_bias_goal = 0.0f;
		float steer_bias = 0.0f;

	public:
		explicit CommandDrunkMode(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}