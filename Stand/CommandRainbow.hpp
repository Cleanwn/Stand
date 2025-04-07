#pragma once

#include "CommandSlider.hpp"

#include "CommandColourCustom.hpp"
#include "Spinlock.hpp"
#include "nullsub.hpp"

namespace Stand
{
	class CommandRainbow;

	using hue_change_handler_t = void(*)(CommandRainbow*);

	struct RainbowData
	{
		time_t ms_wait;
		time_t last_frame;
		time_t ms_passed = 0ll;
	};

	class CommandRainbow : public CommandSlider
	{
	private:
		static inline std::unordered_map<CommandRainbow*, RainbowData> active_rainbows = {};
		static inline std::unordered_map<long long, int> tickrate_hue_map = {};
		static inline uint8_t ref_count = 0;

	protected:
		CommandColourCustom* const colour;
	private:
		hue_change_handler_t hue_change_handler;

	public:
		explicit CommandRainbow(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, CommandColourCustom* colour, hue_change_handler_t hue_change_handler = &nullsub<>);

		void onChange(Click& click, int prev_value) final;

		[[nodiscard]] std::string getState() const final;
		[[nodiscard]] std::string getDefaultState() const final;
		void setState(Click& click, const std::string& state) final;

	private:
		static void onTick();
	};
}
