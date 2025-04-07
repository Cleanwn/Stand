#pragma once

#include <cstdint>

#ifdef STAND_DEBUG
#include <soup/base.hpp>
#include "FileLogger.hpp"
#endif

namespace rage
{
#define POOL_FLAG_ISFREE 0x80
#define POOL_FLAG_REFERENCEMASK 0x7f

	struct fwBasePool
	{
		/* 0x00 */ uint8_t* m_aStorage;
		/* 0x08 */ uint8_t* m_aFlags;
		/* 0x10 */ int32_t m_nSize;
		/* 0x14 */ int32_t m_nStorageSize;
		/* 0x18 */ int32_t m_nFirstFreeIndex;
		/* 0x1C */ int32_t m_nLastFreeIndex;
		/* 0x20 */ int32_t m_numSlotsUsed : 30;
		/* 0x20 */ int32_t m_bOwnsArrays : 2;

		[[nodiscard]] void* GetAt(int32_t index) const noexcept
		{
			uint32_t i = (index >> 8);
			if (m_aFlags[i] == (index & 0xff))
			{
				return (void*)&m_aStorage[i * m_nStorageSize];
			}
			return nullptr;
		}

		// Real version is branchless
		[[nodiscard]] void* GetSlot(int32_t index) const noexcept
		{
			if (!GetIsFree(index))
			{
				return (void*)&m_aStorage[index * m_nStorageSize];
			}
			return nullptr;
		}

		[[nodiscard]] int32_t GetIndex(const void* ptr) const noexcept
		{
			const auto i = GetJustIndex(ptr);
			return (i << 8) + m_aFlags[i];
		}

		[[nodiscard]] int32_t GetJustIndex(const void* ptr) const noexcept
		{
			return (int32_t)((((const uint8_t*)ptr) - ((const uint8_t*)&m_aStorage[0])) / m_nStorageSize);
		}

		[[nodiscard]] bool GetIsFree(int32_t index) const noexcept
		{
			return (m_aFlags[index] & POOL_FLAG_ISFREE) != 0;
		}

		[[nodiscard]] uint8_t GetReference(int32_t index) const noexcept
		{
			return (m_aFlags[index] & POOL_FLAG_REFERENCEMASK);
		}
		
		void SetIsFree(int32_t index, bool bIsFree) noexcept
		{
			bIsFree ? (m_aFlags[index] |= POOL_FLAG_ISFREE) : (m_aFlags[index] &= ~POOL_FLAG_ISFREE);
		}

		void SetReference(int32_t index, uint8_t nReference) noexcept
		{
			m_aFlags[index] = (m_aFlags[index] & ~POOL_FLAG_REFERENCEMASK) | (((nReference & POOL_FLAG_REFERENCEMASK) > 1 ? (nReference & POOL_FLAG_REFERENCEMASK) : 1));
		}

		[[nodiscard]] int32_t GetSize() const noexcept
		{
			return m_nSize;
		}
		
		[[nodiscard]] int32_t GetNoOfUsedSpaces() const noexcept
		{
			return m_numSlotsUsed;
		}

		[[nodiscard]] int32_t GetNoOfFreeSpaces() const noexcept
		{
			return m_nSize - m_numSlotsUsed;
		}
	};

	template <typename T>
	struct fwPool : public fwBasePool
	{
		static constexpr int32_t StorageSize = sizeof(T);

		void validateTemplateStorageSize() const noexcept
		{
#ifdef STAND_DEBUG
			SOUP_IF_UNLIKELY (m_nStorageSize != StorageSize)
			{
				::Stand::g_logger.log("m_nStorageSize differs from expected!");
			}
#endif
		}

		[[nodiscard]] T* GetAt(int32_t index) const noexcept
		{
			validateTemplateStorageSize();

			uint32_t i = (index >> 8);
			if (m_aFlags[i] == (index & 0xff))
			{
				return (T*)&m_aStorage[i * StorageSize];
			}
			return nullptr;
		}

		// Real version is branchless
		[[nodiscard]] T* GetSlot(int32_t index) const noexcept
		{
			if (!GetIsFree(index))
			{
				return (T*)&m_aStorage[index * StorageSize];
			}
			return nullptr;
		}

		[[nodiscard]] int32_t GetIndex(const void* ptr) const noexcept
		{
			const auto i = GetJustIndex(ptr);
			return (i << 8) + m_aFlags[i];
		}

		[[nodiscard]] int32_t GetJustIndex(const void* ptr) const noexcept
		{
			validateTemplateStorageSize();

			return (int32_t)((((const uint8_t*)ptr) - ((const uint8_t*)&m_aStorage[0])) / StorageSize);
		}
	};
}
