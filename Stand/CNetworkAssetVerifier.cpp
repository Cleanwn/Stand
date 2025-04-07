#include "CNetworkAssetVerifier.hpp"

#if true
#include "pointers.hpp"

void CNetworkAssetVerifier::refreshCrc()
{
	::Stand::pointers::CNetworkAssetVerifier_RefreshEnvironmentCRC(this);
}
#else
void CNetworkAssetVerifier::refreshCrc()
{
	m_CRC.Set(calculateCrc());
}

int32_t CNetworkAssetVerifier::calculateCrc() const
{
	int32_t crc = 0;

	uint32_t l_interiorProxyCount = m_interiorProxyCount.Get();
	uint32_t l_interiorProxyChecksum = m_interiorProxyChecksum.Get();
	crc = rage::atDataHash(&l_interiorProxyCount, sizeof(l_interiorProxyCount), crc);
	crc = rage::atDataHash(&l_interiorProxyChecksum, sizeof(l_interiorProxyChecksum), crc);

	uint32_t l_scriptSize = m_scriptSize.Get();
	uint32_t l_scriptStoreHash = m_scriptStoreHash.Get();
	crc = rage::atDataHash(&l_scriptSize, sizeof(l_scriptSize), crc);
	crc = rage::atDataHash(&l_scriptStoreHash, sizeof(l_scriptStoreHash), crc);

	uint32_t l_numFiles = m_numFiles.Get();
	uint32_t l_fileChecksum = m_fileChecksum.Get();
	crc = rage::atDataHash(&l_numFiles, sizeof(l_numFiles), crc);
	crc = rage::atDataHash(&l_fileChecksum, sizeof(l_fileChecksum), crc);

	uint32_t l_nLocalTunesablesCrc = m_nLocalTunablesCRC.Get();
	crc = rage::atDataHash(&l_nLocalTunesablesCrc, sizeof(l_nLocalTunesablesCrc), crc);

	if constexpr (true)
	{
		uint32_t nJapaneseHash = (m_bJapaneseBuild ? 0x7A9A435E : 0);
		crc = rage::atDataHash(&nJapaneseHash, sizeof(nJapaneseHash), crc);
	}

	uint32_t l_memoryTamperCRC = m_memoryTamperCRC.Get(); // ideally this is always 0 lol
	crc = rage::atDataHash(&l_memoryTamperCRC, sizeof(l_memoryTamperCRC), crc);

	uint32_t l_nBackgroundScriptCRC = m_nBackgroundScriptCRC.Get();
	crc = rage::atDataHash(&l_nBackgroundScriptCRC, sizeof(l_nBackgroundScriptCRC), crc);

	uint32_t l_EnvironmentCRC = m_EnvironmentCRC.Get();
	crc = rage::atDataHash(&l_EnvironmentCRC, sizeof(l_EnvironmentCRC), crc);

	return crc;
}
#endif
