#pragma once

#include <cstdint>

#include "sysMemSimpleAllocator.hpp"
#include "tlsContext.hpp"

namespace rage
{
	inline uint32_t atHash64(uint64_t key)
	{
		uint32_t h = 0, g;
		uint32_t count = 8;
		while (--count)
		{
			auto s = (int8_t)key;
			key = key >> 8;
			h = (h << 4) + s;
			if (((g = h & 0xf0000000)) != 0)
			{
				h = h ^ (g >> 24);
				h = h ^ g;
			}
		}
		return h;
	}

	template <typename Key, typename Value>
	struct atMapEntry
	{
		Key key;
		Value data;
		atMapEntry* next;
	};
	static_assert(((uintptr_t)&((atMapEntry<uint32_t, uint32_t>*)0)->next) == 8);

	template <typename Key, typename Value>
	struct atMap
	{
		using Entry = atMapEntry<Key, Value>;

		/* 0x00 */ Entry** m_Hash;
		/* 0x08 */ uint16_t m_Slots;
		/* 0x0A */ uint16_t m_Used;

		[[nodiscard]] Value* accessWithHash64(uint64_t key)
		{
			if (m_Slots != 0)
			{
				Entry* n = m_Hash[atHash64(key) % m_Slots];
				while (n)
				{
					if (n->key == key)
					{
						return &n->data;
					}
					n = n->next;
				}
			}
			return nullptr;
		}

		[[nodiscard]] Value* accessDirect(Key key)
		{
			static_assert(std::is_unsigned_v<Key>);
			if (m_Slots != 0)
			{
				Entry* n = m_Hash[key % m_Slots];
				while (n)
				{
					if (n->key == key)
					{
						return &n->data;
					}
					n = n->next;
				}
			}
			return nullptr;
		}

		void insert(Key key, Value value)
		{
			if (!m_Slots)
			{
				m_Hash = (Entry**)rage::tlsContext::get()->sysMemAllocator_sm_Container->Allocate(sizeof(Entry*) * 191, 16, 0);
				memset(m_Hash, 0, sizeof(Entry*) * 191);
				m_Slots = 191;
			}

			++m_Used;

			auto entry = (Entry*)rage::tlsContext::get()->sysMemAllocator_sm_Container->Allocate(sizeof(Entry), 16, 0);
			entry->key = key;
			entry->data = value;
			entry->next = nullptr;

			Entry** pos = &m_Hash[key % m_Slots];
			while (*pos)
			{
				pos = &(*pos)->next;
			}

			*pos = entry;
		}
	};
}
