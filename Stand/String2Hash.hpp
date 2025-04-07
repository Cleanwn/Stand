#pragma once

#include <soup/base.hpp>

#include "hashtype.hpp"

namespace Stand
{
	union String2Hash
	{
		const char* str;
		struct
		{
			hash_t hash;
			hash_t remain;
		};

		constexpr String2Hash(const char* str)
			: str(str)
		{
		}

		[[nodiscard]] constexpr bool isInHashForm() const noexcept
		{
			return remain == 0;
		}

		operator hash_t() const noexcept
		{
			return hash;
		}

		void convertToHash();
	};
	static_assert(sizeof(String2Hash) == sizeof(const char*));
}
