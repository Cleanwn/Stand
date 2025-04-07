#pragma once

#include "sysObfuscated.hpp"

namespace rage
{
#pragma pack(push, 1)
	class scrNativeRegistrationSubtable // made up name
	{
	public:
		/* 0x00 */ sysObfuscated_U64 next;
		/* 0x10 */ scrNativeHandler values[7];
		/* 0x48 */ sysObfuscated_U32 num_entries;
		PAD(0x50, 0x54) sysObfuscated_U64 keys[7];

		[[nodiscard]] scrNativeHandler getHandler(const scrNativeHash native_hash) const noexcept
		{
			for (const scrNativeRegistrationSubtable* subtable = this; subtable != nullptr;
				subtable = (const scrNativeRegistrationSubtable*)subtable->next.getValue()
				)
			{
				for (unsigned int i = 0; i != subtable->num_entries.getValue(); ++i)
				{
					if (subtable->keys[i].getValue() == native_hash)
					{
						return subtable->values[i];
					}
				}
			}
			return nullptr;
		}

		void getKeys(std::vector<scrNativeHash>& keys) const
		{
			for (const scrNativeRegistrationSubtable* subtable = this; subtable != nullptr;
				subtable = (const scrNativeRegistrationSubtable*)subtable->next.getValue()
				)
			{
				for (unsigned int i = 0; i != subtable->num_entries.getValue(); ++i)
				{
					keys.emplace_back(subtable->keys[i].getValue());
				}
			}
		}

		void getNatives(std::unordered_map<scrNativeHash, scrNativeHandler>& map) const
		{
			for (const scrNativeRegistrationSubtable* subtable = this; subtable != nullptr;
				subtable = (const scrNativeRegistrationSubtable*)subtable->next.getValue()
				)
			{
				for (unsigned int i = 0; i != subtable->num_entries.getValue(); ++i)
				{
					map.emplace(subtable->keys[i].getValue(), subtable->values[i]);
				}
			}
		}

		[[nodiscard]] size_t getTotalValues() const noexcept
		{
			size_t accum = 0;
			for (const scrNativeRegistrationSubtable* subtable = this; subtable != nullptr;
				subtable = (const scrNativeRegistrationSubtable*)subtable->next.getValue()
				)
			{
				accum += subtable->num_entries.getValue();
			}
			return accum;
		}
	};
	static_assert(sizeof(scrNativeRegistrationSubtable) == 0x54 + (0x10 * 7));

	class scrNativeRegistrationTable // made up name
	{
	public:
		scrNativeRegistrationSubtable* subtables[256];
		uint32_t unk;
		bool initialised;

		[[nodiscard]] scrNativeHandler getHandler(const scrNativeHash native_hash) const noexcept
		{
			return subtables[(uint8_t)native_hash]->getHandler(native_hash);
		}

		[[nodiscard]] std::vector<scrNativeHash> getKeys()
		{
			std::vector<scrNativeHash> keys;
			for (const auto& subtable : subtables)
			{
				subtable->getKeys(keys);
			}
			return keys;
		}
		
		[[nodiscard]] std::unordered_map<scrNativeHash, scrNativeHandler> getNatives()
		{
			std::unordered_map<scrNativeHash, scrNativeHandler> map;
			for (const auto& subtable : subtables)
			{
				subtable->getNatives(map);
			}
			return map;
		}

		[[nodiscard]] size_t getTotalValues() const noexcept
		{
			size_t accum = 0;
			for (const auto& subtable : subtables)
			{
				accum += subtable->getTotalValues();
			}
			return accum;
		}
	};
#pragma pack(pop)
}
