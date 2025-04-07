#pragma once

#include <string>

#include "atArray.hpp"
#include "sysObfuscated.hpp"

struct MemoryCheck
{
	static constexpr uint32_t MAGIC_XOR_VALUE = 0xB7AC4B1C;

	rage::sysObfuscated_Mutate<uint32_t> nVersionAndType;
	rage::sysObfuscated_Mutate<uint32_t> nAddressStart;
	rage::sysObfuscated_Mutate<uint32_t> nSize;
	rage::sysObfuscated_Mutate<uint32_t> nValue; // = bonus itemId
	rage::sysObfuscated_Mutate<uint32_t> nFlags;
	rage::sysObfuscated_Mutate<uint32_t> nXorCrc;

	void updateXorCrc()
	{
		nXorCrc.Set(
			nVersionAndType.Get() ^
			nAddressStart.Get() ^
			nSize.Get() ^
			nFlags.Get() ^
			nValue.Get()
		);
	}
};

class CTunables
{
	PAD(0, 0x70) rage::atArray<MemoryCheck> memory_checks;
	PAD(0x70 + sizeof(rage::atArray<MemoryCheck>), 0x8C) unsigned int cloud_request_id;

#ifdef STAND_DEBUG
	[[nodiscard]] static CTunables& GetInstance();
#endif

	// ATK: Prevent RtmaPlugin from doing any checks.
	void disableRtmaChecks();

	// ATK: Modify memory_checks then update verify hash to prevent TunablesVerifier crying.
	[[nodiscard]] std::string getVerifyHash() const;
#ifdef STAND_DEBUG
	void updateVerifyHash();
#endif
};
static_assert(sizeof(CTunables) == 0x8C + 4);
