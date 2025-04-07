#include "WaterQuad.hpp"

#include "pointers.hpp"

using namespace Stand;

WaterQuad* WaterQuad::at(int16_t x, int16_t y)
{
	WaterQuad* water_quads = *pointers::water_quads;
	const uint16_t water_quads_size = *pointers::water_quads_size;
	for (uint16_t i = 0; i < water_quads_size; i++)
	{
		if (water_quads[i].minX <= x && x < water_quads[i].maxX
			&& water_quads[i].minY <= y && y < water_quads[i].maxY)
		{
			return &water_quads[i];
		}
	}
	return nullptr;
}

std::vector<uint16_t> WaterQuad::idsAt(int16_t x, int16_t y)
{
	std::vector<uint16_t> res = {};
	WaterQuad* water_quads = *pointers::water_quads;
	const uint16_t water_quads_size = *pointers::water_quads_size;
	for (uint16_t i = 0; i < water_quads_size; i++)
	{
		if (water_quads[i].minX <= x && x < water_quads[i].maxX
			&& water_quads[i].minY <= y && y < water_quads[i].maxY)
		{
			res.emplace_back(i);
		}
	}
	return res;
}

WaterQuad* WaterQuad::get(uint16_t id)
{
	return &(*pointers::water_quads)[id];
}

uint16_t WaterQuad::size()
{
	return *pointers::water_quads_size;
}
