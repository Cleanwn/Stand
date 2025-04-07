#pragma once

#include "CommandToggle.hpp"

#include <soup/base64.hpp>
#include <soup/Packet.hpp>

#include "CExtraContentManager.hpp"
#include "CNetworkAssetVerifier.hpp"
#include "pointers.hpp"

#ifdef STAND_DEBUG
#include "FileLogger.hpp"
#endif

namespace Stand
{
	class CommandFixAssetHashes : public CommandToggle
	{
	private:
		std::string og_hashes_bin;

	public:
		explicit CommandFixAssetHashes(CommandList* const parent)
			: CommandToggle(parent, LOC("ASTFIX"), CMDNAMES_OBF("fixassethashes"), LOC("ASTFIX_H"))
		{
		}

		void onEnable(Click& click) final
		{
			if (!click.isBulk() && !click.isBasicEdition())
			{
				return;
			}

			AssetHashes og_hashes;
			og_hashes.m_fileChecksum = (*pointers::asset_verifier)->m_fileChecksum.Get();
			og_hashes.m_numFiles = (*pointers::asset_verifier)->m_numFiles.Get();
			og_hashes.m_interiorProxyChecksum = (*pointers::asset_verifier)->m_interiorProxyChecksum.Get();
			og_hashes.m_interiorProxyCount = (*pointers::asset_verifier)->m_interiorProxyCount.Get();
			og_hashes.m_nLocalTunablesCRC = (*pointers::asset_verifier)->m_nLocalTunablesCRC.Get();
			og_hashes.m_scriptSize = (*pointers::asset_verifier)->m_scriptSize.Get();
			og_hashes.m_scriptStoreHash = (*pointers::asset_verifier)->m_scriptStoreHash.Get();
			og_hashes.m_scriptRpfHash = (*pointers::asset_verifier)->m_scriptRpfHash.Get();
			og_hashes.extra_content_crc = hooks::CExtraContentManager_GetCRC(*pointers::extra_content_manager, 0);
			og_hashes_bin = og_hashes.toBinaryString();
#ifdef STAND_DEBUG
			if (!click.isAuto())
			{
				g_logger.log(fmt::format("Captured OG asset hashes: {}", soup::base64::encode(og_hashes_bin)));

				g_logger.log(fmt::format("- m_fileChecksum: {}", (*pointers::asset_verifier)->m_fileChecksum.Get()));
				g_logger.log(fmt::format("- m_numFiles: {}", (*pointers::asset_verifier)->m_numFiles.Get()));
				g_logger.log(fmt::format("- m_interiorProxyChecksum: {}", (*pointers::asset_verifier)->m_interiorProxyChecksum.Get()));
				g_logger.log(fmt::format("- m_interiorProxyCount: {}", (*pointers::asset_verifier)->m_interiorProxyCount.Get()));
				g_logger.log(fmt::format("- m_nLocalTunablesCRC: {}", (*pointers::asset_verifier)->m_nLocalTunablesCRC.Get()));
				g_logger.log(fmt::format("- m_scriptSize: {}", (*pointers::asset_verifier)->m_scriptSize.Get()));
				g_logger.log(fmt::format("- m_scriptStoreHash: {}", (*pointers::asset_verifier)->m_scriptStoreHash.Get()));
				//g_logger.log(fmt::format("- m_memoryTamperCRC: {}", (*pointers::asset_verifier)->m_memoryTamperCRC.Get()));
				//g_logger.log(fmt::format("- m_nBackgroundScriptCRC: {}", (*pointers::asset_verifier)->m_nBackgroundScriptCRC.Get()));
				//g_logger.log(fmt::format("- m_EnvironmentCRC: {}", (*pointers::asset_verifier)->m_EnvironmentCRC.Get()));
				//g_logger.log(fmt::format("- m_CRC: {}", (*pointers::asset_verifier)->m_CRC.Get()));

				g_logger.log(fmt::format("- Extra Content CRC: {}", hooks::CExtraContentManager_GetCRC(*pointers::extra_content_manager, 0)));
			}
#endif

			g_hooking.fix_asset_hashes = true;
			setLegitValues();

			// Some people expect this feature to fix this, so I guess we shall oblige.
			(*pointers::extra_content_manager)->m_everHadBadPackOrder = false;
		}

		static void setLegitValues()
		{
			setHashes(soup::base64::decode("VBDXIgAAA8da0Xx1AAAB2AAAAAAAAAAATtv7BwAAAACtdCE3"));
			(*pointers::asset_verifier)->m_memoryTamperCRC.Set(0);
			(*pointers::asset_verifier)->refreshCrc();
		}

		void onDisable(Click& click) final
		{
			g_hooking.fix_asset_hashes = false;

			setHashes(og_hashes_bin);
			(*pointers::asset_verifier)->refreshCrc();
		}

	private:
		static void setHashes(const std::string& hashes_bin)
		{
			AssetHashes hashes;
			SOUP_ASSERT(hashes.fromBinary(hashes_bin));

			(*pointers::asset_verifier)->m_fileChecksum.Set(hashes.m_fileChecksum);
			(*pointers::asset_verifier)->m_numFiles.Set(hashes.m_numFiles);
			(*pointers::asset_verifier)->m_interiorProxyChecksum.Set(hashes.m_interiorProxyChecksum);
			(*pointers::asset_verifier)->m_interiorProxyCount.Set(hashes.m_interiorProxyCount);
			(*pointers::asset_verifier)->m_nLocalTunablesCRC.Set(hashes.m_nLocalTunablesCRC);
			(*pointers::asset_verifier)->m_scriptSize.Set(hashes.m_scriptSize);
			(*pointers::asset_verifier)->m_scriptStoreHash.Set(hashes.m_scriptStoreHash);
			(*pointers::asset_verifier)->m_scriptRpfHash.Set(hashes.m_scriptRpfHash);

			g_hooking.legit_extra_content_crc = hashes.extra_content_crc;
		}

		SOUP_PACKET(AssetHashes)
		{
			u32 m_fileChecksum;
			u32 m_numFiles;
			u32 m_interiorProxyChecksum;
			u32 m_interiorProxyCount;
			u32 m_nLocalTunablesCRC;
			u32 m_scriptSize;
			u32 m_scriptStoreHash;
			u32 m_scriptRpfHash;
			u32 extra_content_crc;

			SOUP_PACKET_IO(s)
			{
				return s.u32(m_fileChecksum)
					&& s.u32(m_numFiles)
					&& s.u32(m_interiorProxyChecksum)
					&& s.u32(m_interiorProxyCount)
					&& s.u32(m_nLocalTunablesCRC)
					&& s.u32(m_scriptSize)
					&& s.u32(m_scriptStoreHash)
					&& s.u32(m_scriptRpfHash)
					&& s.u32(extra_content_crc)
					;
			}
		};
	};
}
