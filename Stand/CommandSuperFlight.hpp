#pragma once

#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandSuperFlight : public CommandSliderFloat
	{
	private:
		enum Mode : uint8_t
		{
			DEFAULT,
			SUPER_JUMP_TO_FLIGHT,
			FLYING,
			DISABLING
		};

		Mode mode = DEFAULT;
		bool enabled_graceful_landing = false;
		bool enabled_gracefulness = false;

	public:
		bool bi_jump2fly = true;
		bool bi_controls = true;

		explicit CommandSuperFlight(CommandList* const parent);

		std::wstring formatNumber(int num, bool allow_replacements) const final;

		void onChange(Click& click, int prev_value) final;
	private:
		[[nodiscard]] static float getDistanceToGround();
		void onTickStartup();
		void onTickFlying();
		void reset();
	};
}
