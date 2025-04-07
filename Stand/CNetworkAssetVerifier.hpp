#pragma once

#include "sysObfuscated.hpp"

#include <soup/joaat.hpp>

namespace rage
{
	[[nodiscard]] inline uint32_t atDataHash(const uint32_t* data, size_t size, const uint32_t initValue) noexcept
	{
		size >>= 2;
		uint32_t key = initValue;
		for (size_t count = 0; count < size; count++)
		{
			key += data[count];
			key += (key << 10);
			key ^= (key >> 6);
		}
		soup::joaat::finalise(key);
		return key;
	}
}

class CNetworkAssetVerifier
{
public:
	bool m_bJapaneseBuild;
	rage::sysObfuscated_Mutate<uint32_t> m_fileChecksum;
	rage::sysObfuscated_Mutate<uint32_t> m_numFiles;
	rage::sysObfuscated_Mutate<uint32_t> m_interiorProxyChecksum;
	rage::sysObfuscated_Mutate<uint32_t> m_interiorProxyCount;
	rage::sysObfuscated_Mutate<uint32_t> m_nLocalTunablesCRC;
	rage::sysObfuscated_Mutate<uint32_t> m_scriptSize;
	rage::sysObfuscated_Mutate<uint32_t> m_scriptStoreHash;
	rage::sysObfuscated_Mutate<uint32_t> m_scriptRpfHash;
	rage::sysObfuscated_Mutate<uint32_t> m_memoryTamperCRC;
	rage::sysObfuscated_Mutate<uint32_t> m_nBackgroundScriptCRC;
	rage::sysObfuscated_Mutate<uint32_t> m_EnvironmentCRC;
	PAD(0xB4, 0xD4) rage::sysObfuscated_Mutate<int32_t> m_CRC;
	/* 0xE4 */ rage::sysObfuscated_Mutate<int32_t> m_StaticCRC; // based on m_EnvironmentCRC
	/* 0xF4 */ rage::sysObfuscated_Mutate<int32_t> m_BattlEye;

	void refreshCrc();
	[[nodiscard]] int32_t calculateCrc() const;
};
static_assert(offsetof(CNetworkAssetVerifier, m_memoryTamperCRC) == 0x84); // 2845, might be wrong for 2944
static_assert(offsetof(CNetworkAssetVerifier, m_CRC) == 0xD4); // 2944
