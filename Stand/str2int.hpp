#pragma once

#include <optional>
#include <string>

#include "is_number_char.hpp"

namespace Stand
{
	template <typename IntT, typename CharT>
	[[nodiscard]] __forceinline IntT str2int_it(const CharT*& it) noexcept
	{
		IntT val = 0;
		IntT max = 0;
		IntT prev_max = 0;
		while (true)
		{
			const CharT c = *it++;
			if (!is_number_char(c))
			{
				break;
			}
			val *= 10;
			val += (c - '0');
			max *= 10;
			max += 9;
			if (max < prev_max)
			{
				break;
			}
			prev_max = max;
		}
		return val;
	}

	template <typename IntT, typename StringView>
	[[nodiscard]] __forceinline std::optional<IntT> str2int_impl(const StringView& str) noexcept
	{
		using CharT = StringView::value_type;

		bool neg = false;
		auto it = str.cbegin();
		if (it == str.cend())
		{
			return std::nullopt;
		}
		switch (*it)
		{
		case '-':
			neg = true;
			[[fallthrough]];
		case '+':
			if (++it == str.cend())
			{
				return std::nullopt;
			}
		}
		if (!is_number_char(*it))
		{
			return std::nullopt;
		}
		const CharT* it_ = &*it;
		IntT val = str2int_it<IntT, CharT>(it_);
		if (neg)
		{
			val *= -1;
		}
		return std::optional<IntT>(std::move(val));
	}

	template <typename IntT>
	[[nodiscard]] std::optional<IntT> str2int(const std::string_view& str) noexcept
	{
		return str2int_impl<IntT, std::string_view>(str);
	}

	template <typename IntT>
	[[nodiscard]] std::optional<IntT> str2int(const std::wstring_view& str) noexcept
	{
		return str2int_impl<IntT, std::wstring_view>(str);
	}

	template <typename IntT>
	[[nodiscard]] IntT str2int(const std::string_view& str, IntT default_value) noexcept
	{
		auto res = str2int<IntT>(str);
		if (res.has_value())
		{
			return res.value();
		}
		return default_value;
	}

	template <typename IntT>
	[[nodiscard]] IntT str2int(const std::wstring_view& str, IntT default_value) noexcept
	{
		auto res = str2int<IntT>(str);
		if (res.has_value())
		{
			return res.value();
		}
		return default_value;
	}
}
