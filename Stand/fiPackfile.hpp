#pragma once

namespace rage
{
	struct fiDevice
	{
		virtual ~fiDevice() = default;
	};

	struct fiPackEntry
	{
		static constexpr uint64_t FileOffset_IsDir = (1 << 23) - 1;

		uint64_t m_NameOffset : 16;
		uint64_t m_ConsumedSize : 24;
		uint64_t m_FileOffset : 23;
		uint64_t m_IsResource : 1;
		union
		{
			struct
			{
				uint32_t m_UncompressedSize;
				uint32_t m_Encrypted;
			} file;
			struct
			{
				uint32_t m_DirectoryIndex;
				uint32_t m_DirectoryCount;
			} directory;
			/*struct
			{
				datResourceInfo m_Info;
			} resource;*/
		} u;

		bool IsDir() const { return m_FileOffset == FileOffset_IsDir; }
		bool IsResource() const { return m_IsResource; }
		bool IsFile() const { return !IsDir() && !IsResource(); }
		bool IsCompressed() const { return m_ConsumedSize != 0; }
	};
	static_assert(sizeof(fiPackEntry) == 16);

	struct fiCollection : public fiDevice
	{
		bool m_isStreaming;
	};
	static_assert(sizeof(fiCollection) == 0x10);

	struct fiPackfile : public fiCollection
	{
		virtual ~fiPackfile() = default;

		typedef uint16_t ParentIndexType;

		/* 0x10 */ char* m_NameHeap;
		/* 0x18 */ ParentIndexType* m_ParentIndices;
		/* 0x20 */ fiPackEntry* m_Entries;
		/* 0x28 */ uint32_t m_EntryCount;
		PAD(0x2C, 0xB0) uint32_t m_CachedDataSize;
		/* 0xB4 */ uint32_t m_keyId;
	};
	static_assert(offsetof(fiPackfile, m_NameHeap) == 0x10);
	static_assert(offsetof(fiPackfile, m_CachedDataSize) == 0xB0);
	static_assert(offsetof(fiPackfile, m_keyId) == 0xB4);
}
