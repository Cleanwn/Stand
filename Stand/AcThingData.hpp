#pragma once

#include <soup/joaat.hpp>

#include "atMap.hpp"
#include "sysObfuscated.hpp"

struct AcThingData
{
	/* 0x00 */ rage::sysObfuscated_Mutate<uint32_t> timer_1;
	/* 0x10 */ rage::sysObfuscated_Mutate<uint32_t> timer_1_interval;
	PAD(0x20, 0xA78) rage::atMap<uint32_t, uint32_t> mm_metric_reported_modules;
	/* 0xA88 */ uint32_t mm_metric_last_check;
	/* 0xA8C */ uint32_t mm_metric_interval;

	[[nodiscard]] static uint32_t getModuleHash(const std::wstring& name)
	{
		return soup::joaat::hashRange((const char*)name.data(), name.size() * sizeof(wchar_t));
	}

	[[nodiscard]] bool wasModuleReported(uint32_t hash)
	{
		return mm_metric_reported_modules.accessDirect(hash) != nullptr;
	}

	void markModuleAsReported(uint32_t hash)
	{
		mm_metric_reported_modules.insert(hash, 1);
	}
};
static_assert(offsetof(AcThingData, mm_metric_interval) == 0xA8C);
