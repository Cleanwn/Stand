#pragma once

#include <cstdint>
#include <vector>

#include "gta_fwddecl.hpp"

struct WaterOpacityData
{
	std::uint8_t a1;
	std::uint8_t a2;
	std::uint8_t a3;
	std::uint8_t a4;
};
static_assert(sizeof(WaterOpacityData) == 4);

struct WaterQuad
{
	std::int16_t minX; //0x0000
	std::int16_t minY; //0x0002
	std::int16_t maxX; //0x0004
	std::int16_t maxY; //0x0006
	WaterOpacityData opacity; //0x0008
	float unk; //0x000C
	std::uint8_t pad_0010[4]; //0x0010
	float z; //0x0014
	std::uint8_t unk_0018; //0x0018
	std::uint8_t Type; //0x0019
	std::uint8_t pad_001A[2]; //0x001A

	[[nodiscard]] static WaterQuad* at(int16_t x, int16_t y);
	[[nodiscard]] static WaterQuad* at(float x, float y)
	{
		return at(int16_t(x), int16_t(y));
	}

	[[nodiscard]] static std::vector<uint16_t> idsAt(int16_t x, int16_t y);
	[[nodiscard]] static std::vector<uint16_t> idsAt(float x, float y)
	{
		return idsAt(int16_t(x), int16_t(y));
	}

	[[nodiscard]] static WaterQuad* get(uint16_t id);
	[[nodiscard]] static uint16_t size();

};
static_assert(sizeof(WaterQuad) == 0x001C);
