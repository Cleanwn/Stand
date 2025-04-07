#pragma once

#include <cstdint>

#define RAGE_RTTI_SHIM \
virtual rage::ClassId* getClassId() { return nullptr; } \
virtual rage::ClassId* getClassId_2() { return nullptr; } \
virtual uint32_t getNameHash() { return 0; } \
virtual bool getIsClassId(rage::ClassId* id) { return false; } \
virtual bool getIsClassId(uint64_t id) { return false; } \
virtual bool getIsClassId(uint64_t* id) { return false; }

namespace rage
{
	struct ClassId // made up
	{
		uint64_t id; // equals to `this`, unique enough for an ID, I guess...
		uint32_t name_hash;
	};
}
