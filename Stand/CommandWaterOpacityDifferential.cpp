#include "CommandWaterOpacityDifferential.hpp"

#include <algorithm>

#include "pointers.hpp"

namespace Stand
{
	CommandWaterOpacityDifferential::CommandWaterOpacityDifferential(CommandList* const parent)
		: CommandSlider(parent, LOC("OPCDIF"), CMDNAMES("wateropacity"), LOC("WATOD_H"), 0 - 80, 255 - 22, 0)
	{
	}

	Label CommandWaterOpacityDifferential::getActivationName() const
	{
		return getActivationNameImplCombineWithParent();
	}

	void CommandWaterOpacityDifferential::onChange(Click& click, int prev_value)
	{
		ensureScriptThread(click, [this]
		{
			// Recover original opacity
			{
				const uint16_t water_quads_size = *pointers::water_quads_size;
				if (water_quads_size == og_opacity.size())
				{
					WaterQuad* water_quads = *pointers::water_quads;
					for (uint16_t i = 0; i < water_quads_size; i++)
					{
						water_quads[i].opacity = og_opacity.at(i);
					}
				}
				og_opacity.clear();
			}
			if (value != 0)
			{
				const int value = this->value;
				registerAsyncTickEventHandler(TC_SCRIPT_NOYIELD, [this, value]
				{
					if (value != this->value)
					{
						return false;
					}
					const uint16_t water_quads_size = *pointers::water_quads_size;
					if (water_quads_size != og_opacity.size())
					{
						og_opacity.clear();
						og_opacity.reserve(water_quads_size);
						WaterQuad* water_quads = *pointers::water_quads;
						for (uint16_t i = 0; i < water_quads_size; i++)
						{
							WaterOpacityData opacity = water_quads[i].opacity;
							og_opacity.emplace_back(opacity);
							water_quads[i].opacity.a1 = std::clamp(water_quads[i].opacity.a1 + value, 0, 255);
							water_quads[i].opacity.a2 = std::clamp(water_quads[i].opacity.a2 + value, 0, 255);
							water_quads[i].opacity.a3 = std::clamp(water_quads[i].opacity.a3 + value, 0, 255);
							water_quads[i].opacity.a4 = std::clamp(water_quads[i].opacity.a4 + value, 0, 255);
						}
					}
					return true;
				});
			}
		});
	}
}
