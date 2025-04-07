#pragma once

#include <string>

namespace Stand
{
	[[nodiscard]] extern std::string extract_delimited_string_with_escaping(std::string& str, const char delimiter);
	[[nodiscard]] extern std::wstring extract_delimited_string_with_escaping(std::wstring& str, const wchar_t delimiter);
}
