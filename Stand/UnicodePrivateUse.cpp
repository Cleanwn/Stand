#include "UnicodePrivateUse.hpp"

#include "StringUtils.hpp"

namespace Stand
{
	static constexpr std::pair<const wchar_t*, const wchar_t*> gta_stand_map[] = {
		{L"∑", UnicodePrivateUse::rs},
		{L"~ws~", UnicodePrivateUse::wanted_star},
		{L"Ω", UnicodePrivateUse::lock},
		{L"¦", UnicodePrivateUse::rs_verified},
		{L"‹", UnicodePrivateUse::rs_created},
		{L"›", UnicodePrivateUse::blank_box},
		{L"~n~", UnicodePrivateUse::newline},
		{L"~s~", UnicodePrivateUse::reset},
	};

	void UnicodePrivateUse::fromGta(std::wstring& text)
	{
		StringUtils::replace_all(text, L"~ex_r*~", UnicodePrivateUse::rs);
		StringUtils::replace_all(text, L"~wanted_star~", UnicodePrivateUse::wanted_star);
		for (const auto& sequence_mapping : gta_stand_map)
		{
			StringUtils::replace_all(text, sequence_mapping.first, sequence_mapping.second);
		}
	}

	std::wstring UnicodePrivateUse::fromGta(std::wstring&& text)
	{
		fromGta(text);
		return text;
	}

	void UnicodePrivateUse::toGta(std::wstring& text)
	{
		for (const auto& sequence_mapping : gta_stand_map)
		{
			StringUtils::replace_all(text, sequence_mapping.second, sequence_mapping.first);
		}
	}

	std::wstring UnicodePrivateUse::toGta(std::wstring&& text)
	{
		toGta(text);
		return text;
	}

	void UnicodePrivateUse::destroyGta(std::wstring& text)
	{
		StringUtils::replace_all(text, L"~ex_r*~", L"?");
		StringUtils::replace_all(text, L"~wanted_star~", L"?");
		for (const auto& sequence_mapping : gta_stand_map)
		{
			StringUtils::replace_all(text, sequence_mapping.first, L"?");
		}
	}
}
