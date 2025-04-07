#pragma once

#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandWaterHeight : public CommandSliderFloat
	{
	private:
		std::unordered_map<uint16_t, float> og_heights = {};

	public:
		CommandToggleNoCorrelation* local;

		explicit CommandWaterHeight(CommandList* const parent);

	private:
		void setWaterQuadHeightDifferential(uint16_t water_quad_id, float height_diff);
	public:
		void onChange(Click& click, int prev_value) final;
	};
}
