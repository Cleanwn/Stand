#pragma once

#include <cstdint>

namespace rage
{
	// template<class T, bool TMutate=true> class sysObfuscated
	// slightly expanded here for improved performance

	template<class T> class sysObfuscated_Mutate
	{
	private:
		uint32_t m_data[(sizeof(T) * 2) / sizeof(uint32_t)];
		uint32_t m_xor;
		uint32_t m_mutate;

	public:
		T Get() const;
		void Set(const T& data);
	};

	template<class T> T sysObfuscated_Mutate<T>::Get() const
	{
		uint32_t xorVal = m_xor ^ (uint32_t)(size_t)this;
		uint32_t ret[sizeof(T) / sizeof(uint32_t)];
		uint32_t* src = const_cast<uint32_t*>(&m_data[0]);
		uint32_t* dest = (uint32_t*)&ret;
		for (size_t i = 0; i < sizeof(T) / 4; ++i)
		{
			// Extract valid data from two words of storage
			uint32_t a = *src & m_mutate;
			uint32_t b = src[sizeof(T) / 4] & (~m_mutate);

			if constexpr (false)
			{
				// Apply entropy in the unused bits: Just flip the two u16's in the uint32_t. We can't do a
				// huge amount more without knowledge of the mutation mask.
				uint32_t entropyA = ((*src & (~m_mutate)) << 16) | ((*src & (~m_mutate)) >> 16);
				uint32_t entropyB = ((src[sizeof(T) / 4] & m_mutate) << 16) | ((src[sizeof(T) / 4] & m_mutate) >> 16);
				*src = (*src & m_mutate) | entropyA;
				src[sizeof(T) / 4] = (src[sizeof(T) / 4] & (~m_mutate)) | entropyB;
			}

			*dest++ = a | b;
			++src;
		}
		return *(T*)&ret;
	}

	template<class T> void sysObfuscated_Mutate<T>::Set(const T& data)
	{
		uint32_t xorVal = m_xor ^ (uint32_t)(size_t)this;
		uint32_t* src = (uint32_t*)&data;
		uint32_t* dest = &m_data[0];
		for (size_t i = 0; i < sizeof(T) / 4; ++i)
		{
			uint32_t a = *src & m_mutate;
			uint32_t b = *src & (~m_mutate);
			++src;

			*dest = a;
			dest[sizeof(T) / 4] = b;
			++dest;
		}
	}

	struct sysObfuscated_U32
	{
		/* 0x0 */ uint32_t first;
		/* 0x4 */ uint32_t second;

		[[nodiscard]] uint32_t getValue() const noexcept
		{
			return first ^ (uint32_t)(uintptr_t)this ^ second;
		}
	};
	static_assert(sizeof(sysObfuscated_U32) == 0x8);

	struct sysObfuscated_U64
	{
		/* 0x0 */ uint32_t enc_lo;
		/* 0x4 */ uint32_t enc_hi;
		/* 0x8 */ uint32_t enc_key;
		/* 0xC */ uint32_t pad;

		[[nodiscard]] uint64_t getValue() const noexcept
		{
			uint32_t key = (enc_key ^ (uint32_t)(uintptr_t)this);
			return ((uint64_t)(enc_hi ^ key) << 32) | (enc_lo ^ key);
		}
	};
	static_assert(sizeof(sysObfuscated_U64) == 0x10);
}
