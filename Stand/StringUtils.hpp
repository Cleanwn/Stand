#pragma once

#include <string>
#include <vector>

#include <soup/string.hpp>

#include "StringCastable.hpp"

namespace Stand
{
	struct StringUtils
	{
		[[nodiscard]] static std::string float_to_string_with_precision(const float f, const int precision);
		[[nodiscard]] static std::wstring utf8_to_utf16(const std::string& utf8) noexcept;
		[[nodiscard]] static std::string utf16_to_utf8(const std::wstring& utf16);
		static void to_lower(std::string& str);
		static void to_lower(std::wstring& str);
		[[nodiscard]] static std::wstring to_lower(std::wstring&& str);
		static void to_upper(std::string& str);
		static void replace_single(std::string& str, const std::string& from, const std::string& to);

		template <class S>
		static void replace_all(S& str, const S& from, const S& to) noexcept
		{
			size_t start_pos = 0;
			while ((start_pos = str.find(from, start_pos)) != S::npos)
			{
				str.replace(start_pos, from.length(), to);
				start_pos += to.length();
			}
		}

		static void replace_all(std::string& str, const std::string& from, const std::string& to) noexcept
		{
			return replace_all<std::string>(str, from, to);
		}

		static void replace_all(std::wstring& str, const std::wstring& from, const std::wstring& to) noexcept
		{
			return replace_all<std::wstring>(str, from, to);
		}

		// simplify: Ensures string is lowercase and removes all non-alphanumeric characters.
		// simplifyKeepCase: Removes all non-alphanumeric characters.
		// simplifyCommandName: Ensures string is lowercase and removes space characters.

		static void simplify(std::string& str);
		[[nodiscard]] static std::string simplify(const std::string& str);
		[[nodiscard]] static std::string simplify(std::string&& str);
		static void simplify(std::wstring& str);
		[[nodiscard]] static std::wstring simplify(const std::wstring& str);
		[[nodiscard]] static std::wstring simplify(std::wstring&& str);
		static void simplifyCommandName(std::string& str);
		[[nodiscard]] static std::string simplifyCommandName(const std::string& str);
		[[nodiscard]] static std::string simplifyCommandName(std::string&& str);
		static void simplifyCommandName(std::wstring& str);

		static void list_append(std::string& str, const StringCastable& add);
		static void list_append_nl(std::string& str, std::string&& add);
		static void list_append_safe(std::string& str, std::string&& add);
		static void list_append(std::wstring& str, const std::wstring& add);
		[[nodiscard]] static std::string toStringWithThousandsSeparator(uint64_t num, bool negative = false);
		[[nodiscard]] static std::wstring toStringWithThousandsSeparatorW(uint64_t num, bool negative = false);
		[[nodiscard]] static std::wstring toStringWithThousandsSeparatorW(int64_t num);
		[[nodiscard]] static std::wstring toStringWithThousandsSeparatorW(uint32_t num, bool negative = false);
		[[nodiscard]] static std::wstring toStringWithThousandsSeparatorW(int32_t num);

		[[nodiscard]] static std::vector<std::wstring> explode_with_delimiting(std::wstring str, const wchar_t delim);

		static void mock_mutable(std::string& str);
		[[nodiscard]] static std::string mock(std::string str);
		[[nodiscard]] static std::string owoify(const std::string& str);
		[[nodiscard]] static std::wstring owoify(const std::wstring& str);
		[[nodiscard]] static std::wstring owoifyWithFmtException(const std::wstring& str);
		[[nodiscard]] static bool containsWord(const std::wstring& haystack, const std::wstring& needle);
		[[nodiscard]] static std::string concatListUsingLang(std::vector<std::string>&& items);

		// Handles strings having a random number at front or back (or other small modifications).
		template <typename T = std::string>
		[[nodiscard]] static bool isBasicallyTheSame(const T& a, const T& b)
		{
			if (a == b)
			{
				return true;
			}
			if (a.size() > 10 && b.size() > 10)
			{
				return soup::string::levenshtein(a, b) < 4;
			}
			return false;
		}

		[[nodiscard]] static std::wstring removeSpaces(std::wstring msg);
		[[nodiscard]] static std::wstring removeSpacesAndPunctuation(std::wstring msg);
		static void removeSpacesAroundPunctuation(std::wstring& msg);
	};
}
