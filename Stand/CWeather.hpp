#pragma once

#include "struct_base.hpp"

struct CWeather
{
	PAD(0, 0xE9C) float m_rain;
	PAD(0xE9C + 4, 0xEC0) float m_wetness;
};
// local g_weather = memory.rip(memory.scan("48 03 C1 EB 0C 48 8D 0D") + 8)
