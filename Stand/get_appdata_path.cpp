#include "get_appdata_path.hpp"

#include <soup/ObfusString.hpp>

#include "StringUtils.hpp"

namespace Stand
{
	std::wstring get_roaming_path()
	{
		std::wstring path = StringUtils::utf8_to_utf16(soup::ObfusString("appdata").str());
		path = _wgetenv(path.c_str());
		path.push_back(L'\\');
		return path;
	}

	std::wstring get_appdata_path()
	{
		auto path = get_roaming_path();
		path.append(StringUtils::utf8_to_utf16(soup::ObfusString("Stand\\").str()));
		return path;
	}

	std::wstring get_appdata_path(const std::string& rel)
	{
		return get_appdata_path(StringUtils::utf8_to_utf16(rel));
	}

	std::wstring get_appdata_path(const std::wstring& rel)
	{
		std::wstring path = get_appdata_path();
		path.append(rel);
		return path;
	}
}
