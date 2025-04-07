#pragma once

#include <string>

#include <fmt/core.h>
#include <fmt/xchar.h>

#include <soup/macros.hpp>
#include "typedecl.hpp"

#include "lang.hpp"

namespace Stand
{
	template <typename T>
	class BiString : public T
	{
	public:
		using T::T;

		BiString(T&& t)
			: T(std::move(t))
		{
		}


		static constexpr bool isA = std::is_same_v<T, std::string>;

		static BiString<T> fromLang(hash_t hash)
		{
			if constexpr (isA)
			{
				return BiString<T>(Lang::get(hash));
			}
			else
			{
				return BiString<T>(Lang::getW(hash));
			}
		}
		
		template <typename V>
		static BiString<T> fromValue(V val)
		{
			if constexpr (isA)
			{
				return BiString<T>(fmt::to_string(val));
			}
			else
			{
				return BiString<T>(fmt::to_wstring(val));
			}
		}

#define BI_APPEND(...) MULTI_MACRO(BI_APPEND, __VA_ARGS__)
#define BI_APPEND_2(str, ap) \
if constexpr (str.isA) \
{ \
	str.append(ap); \
} \
else \
{ \
	str.append(L##ap); \
}
#define BI_APPEND_3(str, rep, ap) \
if constexpr (str.isA) \
{ \
	str.append(rep, ap); \
} \
else \
{ \
	str.append(rep, L##ap); \
}

#define BI_INSERT(str, pos, ap) \
if constexpr (str.isA) \
{ \
	str.insert(pos, ap); \
} \
else \
{ \
	str.insert(pos, L##ap); \
}

#define BI_PUSH_BACK(str, ap) \
if constexpr (str.isA) \
{ \
	str.push_back(ap); \
} \
else \
{ \
	str.push_back(L##ap); \
}
	};
}
