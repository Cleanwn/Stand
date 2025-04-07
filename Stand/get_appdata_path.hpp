#pragma once

#include <string>

namespace Stand
{
	[[nodiscard]] extern std::wstring get_roaming_path();
	[[nodiscard]] extern std::wstring get_appdata_path();
	[[nodiscard]] extern std::wstring get_appdata_path(const std::string& rel);
	[[nodiscard]] extern std::wstring get_appdata_path(const std::wstring& rel);
}
