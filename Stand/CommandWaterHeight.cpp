#include "CommandWaterHeight.hpp"

#include "AbstractEntity.hpp"
#include "CommandToggleNoCorrelation.hpp"
#include "vector.hpp"
#include "WaterQuad.hpp"

namespace Stand
{
	CommandWaterHeight::CommandWaterHeight(CommandList* const parent)
		: CommandSliderFloat(parent, LOC("ZDIFF"), CMDNAMES("waterheight", "waterzdifferential"), LOC("GEN_H_LOC"), -1000000, 1000000, 0, 100)
	{
	}

	static void processPossibleWaterQuad(std::unordered_map<uint16_t, float>& og_heights, std::unordered_set<uint16_t>& water_quad_ids, const v3& pos)
	{
		auto res = WaterQuad::idsAt(pos.x, pos.y);
		for (auto& water_quad_id : res)
		{
			float water_z;
			auto entry = og_heights.find(water_quad_id);
			if (entry == og_heights.end())
			{
				water_z = WaterQuad::get(water_quad_id)->z;
			}
			else
			{
				water_z = entry->second;
			}
			if (water_z + 10.0f >= pos.z)
			{
				water_quad_ids.emplace(water_quad_id);
			}
		}
	}

	static void processPossibleWaterQuadsInCircle(std::unordered_map<uint16_t, float>& og_heights, std::unordered_set<uint16_t>& water_quad_ids, const v3& pos, float dist)
	{
		for (float deg = 0.0f; deg < 360.0f; deg += (360.0f / 8.0f))
		{
			processPossibleWaterQuad(og_heights, water_quad_ids, (v3(0.0f, 0.0f, deg).toDirNoZ() * dist) + pos);
		}
	}

	void CommandWaterHeight::setWaterQuadHeightDifferential(uint16_t water_quad_id, float height_diff)
	{
		WaterQuad* const water_quad = WaterQuad::get(water_quad_id);
		auto entry = og_heights.find(water_quad_id);
		if (entry == og_heights.end())
		{
			og_heights.emplace(water_quad_id, water_quad->z);
			water_quad->z += height_diff;
		}
		else
		{
			water_quad->z = entry->second + height_diff;
		}
	}

	void CommandWaterHeight::onChange(Click& click, int prev_value)
	{
		registerScriptTickEventHandler(click, [this]
		{
			const bool still_active = (value != 0);
			if (local->m_on)
			{
				std::unordered_set<uint16_t> water_quad_ids{};
				if (still_active)
				{
					const auto pos = g_player_ent.getPos();
					processPossibleWaterQuad(og_heights, water_quad_ids, pos);
					processPossibleWaterQuadsInCircle(og_heights, water_quad_ids, pos, 3.0f);
					processPossibleWaterQuadsInCircle(og_heights, water_quad_ids, pos, 10.0f);
				}
				for (auto i = og_heights.begin(); i != og_heights.end(); )
				{
					if (water_quad_ids.count(i->first) == 0)
					{
						WaterQuad::get(i->first)->z = i->second;
						i = og_heights.erase(i);
					}
					else
					{
						i++;
					}
				}
				for (const auto& water_quad_id : water_quad_ids)
				{
					setWaterQuadHeightDifferential(water_quad_id, getFloatValue());
				}
			}
			else
			{
				if (still_active)
				{
					for (uint16_t i = 0; i != WaterQuad::size(); ++i)
					{
						setWaterQuadHeightDifferential(i, getFloatValue());
					}
				}
				else
				{
					for (auto& og_height : og_heights)
					{
						WaterQuad::get(og_height.first)->z = og_height.second;
					}
					og_heights.clear();
				}
				
			}
			return still_active;
		});
	}
}
