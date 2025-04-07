#include "StringUtils.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include <soup/unicode.hpp>

#include <fmt/format.h>
#include <fmt/xchar.h>

#include "IsolatedText.hpp"
#include "lang.hpp"
#include "UnicodePrivateUse.hpp"

namespace Stand
{
	std::string StringUtils::float_to_string_with_precision(const float f, const int precision)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(precision) << f;
		return stream.str();
	}

	std::wstring StringUtils::utf8_to_utf16(const std::string& utf8) noexcept
	{
		return soup::unicode::utf8_to_utf16(utf8);
	}

	std::string StringUtils::utf16_to_utf8(const std::wstring& utf16)
	{
		return soup::unicode::utf16_to_utf8(utf16);
	}

	void StringUtils::to_lower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](char c) -> char
		{
			// Co-loading 2take1 breaks std::tolower & std::toupper for non-latin characters. You can't make this shit up.
			if (c >= 'A' && c <= 'Z')
			{
				return c - 'A' + 'a';
			}
			return c;
		});
	}

	void StringUtils::to_lower(std::wstring& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](wchar_t c) -> wchar_t
		{
			// Co-loading 2take1 breaks std::tolower & std::toupper for non-latin characters. You can't make this shit up.
			if (c >= 'A' && c <= 'Z')
			{
				return c - 'A' + 'a';
			}
			return c;
		});
	}

	std::wstring StringUtils::to_lower(std::wstring&& str)
	{
		to_lower(str);
		return str;
	}

	void StringUtils::to_upper(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](char c) -> char
		{
			// Co-loading 2take1 breaks std::tolower & std::toupper for non-latin characters. You can't make this shit up.
			if (c >= 'a' && c <= 'z')
			{
				return c - 'a' + 'A';
			}
			return c;
		});
	}

	void StringUtils::replace_single(std::string& str, const std::string& from, const std::string& to)
	{
		size_t start_pos = str.find(from);
		if (start_pos != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
		}
	}

	template <typename Char>
	static bool simplify_predicate(const Char c)
	{
		return !((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
	}

	template <typename Char>
	static bool simplify_command_name_predicate(const Char c)
	{
		return c == ' ';
	}

	void StringUtils::simplify(std::string& str)
	{
		to_lower(str);
		str.erase(std::remove_if(str.begin(), str.end(), &simplify_predicate<char>), str.end());
	}

	std::string StringUtils::simplify(const std::string& str)
	{
		return simplify(std::string(str));
	}

	std::string StringUtils::simplify(std::string&& str)
	{
		simplify(str);
		return str;
	}

	void StringUtils::simplify(std::wstring& str)
	{
		to_lower(str);
		str.erase(std::remove_if(str.begin(), str.end(), &simplify_predicate<wchar_t>), str.end());
	}

	std::wstring StringUtils::simplify(const std::wstring& str)
	{
		return simplify(std::wstring(str));
	}

	std::wstring StringUtils::simplify(std::wstring&& str)
	{
		simplify(str);
		return str;
	}

	void StringUtils::simplifyCommandName(std::string& str)
	{
		to_lower(str);
		str.erase(std::remove_if(str.begin(), str.end(), &simplify_command_name_predicate<char>), str.end());
	}

	std::string StringUtils::simplifyCommandName(const std::string& str)
	{
		return simplifyCommandName(std::string(str));
	}

	std::string StringUtils::simplifyCommandName(std::string&& str)
	{
		simplifyCommandName(str);
		return str;
	}

	void StringUtils::simplifyCommandName(std::wstring& str)
	{
		to_lower(str);
		str.erase(std::remove_if(str.begin(), str.end(), &simplify_command_name_predicate<wchar_t>), str.end());
	}

	void StringUtils::list_append(std::string& str, const StringCastable& add)
	{
		if (str.empty())
		{
			str = add.value;
		}
		else
		{
			str.append(", ").append(add.value);
		}
	}

	void StringUtils::list_append_nl(std::string& str, std::string&& add)
	{
		if (str.empty())
		{
			str = std::move(add);
		}
		else
		{
			str.push_back('\n');
			str.append(add);
		}
	}

	void StringUtils::list_append_safe(std::string& str, std::string&& add)
	{
		replace_all(add, ",", "\\,");
		if (str.empty())
		{
			str = std::move(add);
		}
		else
		{
			str.append(", ").append(add);
		}
	}

	void StringUtils::list_append(std::wstring& str, const std::wstring& add)
	{
		if (str.empty())
		{
			str = add;
		}
		else
		{
			str.append(L", ").append(add);
		}
	}

	std::string StringUtils::toStringWithThousandsSeparator(uint64_t num, bool negative)
	{
		std::string str = fmt::to_string(num);
		size_t i = str.length();
		uint8_t j = 0;
		while (--i > 0)
		{
			if (++j == 3)
			{
				str.insert(i, LANG_GET("THSNDSEP"));
				j = 0;
			}
		}
		if (negative)
		{
			str.insert(0, 1, '-');
		}
		return str;
	}

	std::wstring StringUtils::toStringWithThousandsSeparatorW(uint64_t num, bool negative)
	{
		std::wstring str = fmt::to_wstring(num);
		size_t i = str.length();
		uint8_t j = 0;
		while (--i > 0)
		{
			if (++j == 3)
			{
				str.insert(i, LANG_GET_W("THSNDSEP"));
				j = 0;
			}
		}
		if (negative)
		{
			str.insert(0, 1, L'-');
		}
		return str;
	}

	std::wstring StringUtils::toStringWithThousandsSeparatorW(int64_t num)
	{
		const bool negative = num < 0;
		if (negative)
		{
			num *= -1;
		}
		return toStringWithThousandsSeparatorW((uint64_t)num, negative);
	}

	std::wstring StringUtils::toStringWithThousandsSeparatorW(uint32_t num, bool negative)
	{
		return toStringWithThousandsSeparatorW((uint64_t)num, negative);
	}

	std::wstring StringUtils::toStringWithThousandsSeparatorW(int32_t num)
	{
		return toStringWithThousandsSeparatorW((int64_t)num);
	}

	std::vector<std::wstring> StringUtils::explode_with_delimiting(std::wstring str, const wchar_t delim)
	{
		replace_all(str, L"\\;", UnicodePrivateUse::replace_tmp);
		auto res = soup::string::explode(str, delim);
		for (auto& e : res)
		{
			replace_all(e, UnicodePrivateUse::replace_tmp, L";");
		}
		return res;
	}

	void StringUtils::mock_mutable(std::string& str)
	{
		bool switcher = true;
		for (auto& c : str)
		{
			if (soup::string::isLetter(c))
			{
				if (switcher)
				{
					c = std::toupper(c);
				}
				else
				{
					c = std::tolower(c);
				}
				switcher = !switcher;
			}
		}
	}

	std::string StringUtils::mock(std::string str)
	{
		mock_mutable(str);
		return str;
	}

	std::string StringUtils::owoify(const std::string& str)
	{
		return utf16_to_utf8(owoify(utf8_to_utf16(str)));
	}

	std::wstring StringUtils::owoify(const std::wstring& str)
	{
		// Adapted from https://github.com/mackyclemen/uwu-cpp/blob/master/src/uwulib.h
		std::wstring result{};
		wchar_t prev_char = 0;
		for (const wchar_t* c = str.c_str(); *c != 0; ++c)
		{
			switch (*c)
			{
			case 'L':
			case 'R':
				result.push_back(L'W');
				break;
			case 'l':
			case 'r':
				result.push_back(L'w');
				break;

			case 'o':
			case 'O':
				switch (prev_char)
				{
				case 'n':
				case 'N':
				case 'm':
				case 'M':
					result.append(L"yo");
					break;
				default:
					result.push_back(*c);
				}
				break;
			default:
				result.push_back(*c);
			}
			prev_char = *c;
		}
		return result;
	}

	std::wstring StringUtils::owoifyWithFmtException(const std::wstring& str)
	{
		IsolatedText isol(str);
		isol.text = owoify(isol.text);
		return isol.deisolate();
	}

	bool StringUtils::containsWord(const std::wstring& haystack, const std::wstring& needle)
	{
		return soup::string::containsWord<std::wstring>(haystack, needle);
	}

	std::string StringUtils::concatListUsingLang(std::vector<std::string>&& items)
	{
		if (!items.empty())
		{
			if (items.size() == 1)
			{
				return items.at(0);
			}
			auto rbegin = items.rbegin();
			std::string a = *(rbegin + 1);
			auto it = items.rbegin() + 2;
			while (it != items.rend())
			{
				a.insert(0, ", ").insert(0, *it);
				it++;
			}
			return LANG_FMT("AND", a, *(rbegin + 0));
		}
		return {};
	}

	static const wchar_t spaces[] = {
		L' ', // regular ass space
		L'　', // ideographic space (renders as a long space in GTA, normal & Japanese font)
#if false // There are some more that only work in Japanese font: https://emptycharacter.com/
		L'​', // zero-width space (Normal font: renders as square, Japanese font: renders as normal space)
		L' ', // En Quad (Normal font: renders as square, Japanese font: renders as normal space)
		L' ', // En Space (Normal font: renders as square, Japanese font: renders as normal space)
#endif
	};

	std::wstring StringUtils::removeSpaces(std::wstring msg)
	{
		for (const auto& space : spaces)
		{
			StringUtils::replace_all(msg, std::wstring(1, space), {});
		}
		return msg;
	}

	std::wstring StringUtils::removeSpacesAndPunctuation(std::wstring msg)
	{
		msg = removeSpaces(msg);
		StringUtils::replace_all(msg, std::wstring(1, L'.'), {});
		return msg;
	}

	void StringUtils::removeSpacesAroundPunctuation(std::wstring& msg)
	{
		for (const auto& space : spaces)
		{
			auto pre = std::wstring(1, space);
			pre.insert(0, 1, L'.');
			StringUtils::replace_all(msg, pre, L".");

			auto post = std::wstring(1, space);
			post.push_back(L'.');
			StringUtils::replace_all(msg, post, L".");
		}
	}
}
