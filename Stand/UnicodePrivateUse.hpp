#pragma once

#include <string>

namespace Stand
{
	struct UnicodePrivateUse
	{
		// U+E000..U+F8FF

		static constexpr const wchar_t* rs = L"\uE690";
		static constexpr const wchar_t* wanted_star = L"\uE691";
		static constexpr const wchar_t* lock = L"\uE692";
		static constexpr const wchar_t* rs_verified = L"\uE693";
		static constexpr const wchar_t* rs_created = L"\uE694";
		static constexpr const wchar_t* blank_box = L"\uE695";
		static constexpr const wchar_t* newline = L"\uE696";
		static constexpr const wchar_t* reset = L"\uE697";

		static constexpr const wchar_t* tab_self = L"\uE701";
		static constexpr const wchar_t* tab_vehicle = L"\uE702";
		static constexpr const wchar_t* tab_online = L"\uE703";
		static constexpr const wchar_t* tab_players = L"\uE704";
		static constexpr const wchar_t* tab_world = L"\uE705";
		static constexpr const wchar_t* tab_game = L"\uE706";
		static constexpr const wchar_t* tab_stand = L"\uE707";

		static constexpr const wchar_t* replace_tmp = L"\uE800";

		static constexpr const wchar_t* custom_00 = L"\uE900";
		static constexpr const wchar_t* custom_01 = L"\uE901";
		static constexpr const wchar_t* custom_02 = L"\uE902";
		static constexpr const wchar_t* custom_03 = L"\uE903";
		static constexpr const wchar_t* custom_04 = L"\uE904";
		static constexpr const wchar_t* custom_05 = L"\uE905";
		static constexpr const wchar_t* custom_06 = L"\uE906";
		static constexpr const wchar_t* custom_07 = L"\uE907";
		static constexpr const wchar_t* custom_08 = L"\uE908";
		static constexpr const wchar_t* custom_09 = L"\uE909";
		static constexpr const wchar_t* custom_0A = L"\uE90A";
		static constexpr const wchar_t* custom_0B = L"\uE90B";
		static constexpr const wchar_t* custom_0C = L"\uE90C";
		static constexpr const wchar_t* custom_0D = L"\uE90D";
		static constexpr const wchar_t* custom_0E = L"\uE90E";
		static constexpr const wchar_t* custom_0F = L"\uE90F";
		static constexpr const wchar_t* custom_10 = L"\uE910";
		static constexpr const wchar_t* custom_11 = L"\uE911";
		static constexpr const wchar_t* custom_12 = L"\uE912";
		static constexpr const wchar_t* custom_13 = L"\uE913";
		static constexpr const wchar_t* custom_14 = L"\uE914";
		static constexpr const wchar_t* custom_15 = L"\uE915";
		static constexpr const wchar_t* custom_16 = L"\uE916";
		static constexpr const wchar_t* custom_17 = L"\uE917";
		static constexpr const wchar_t* custom_18 = L"\uE918";
		static constexpr const wchar_t* custom_19 = L"\uE919";
		static constexpr const wchar_t* custom_1A = L"\uE91A";
		static constexpr const wchar_t* custom_1B = L"\uE91B";
		static constexpr const wchar_t* custom_1C = L"\uE91C";
		static constexpr const wchar_t* custom_1D = L"\uE91D";
		static constexpr const wchar_t* custom_1E = L"\uE91E";
		static constexpr const wchar_t* custom_1F = L"\uE91F";

		static void fromGta(std::wstring& text);
		[[nodiscard]] static std::wstring fromGta(std::wstring&& text);
		static void toGta(std::wstring& text);
		[[nodiscard]] static std::wstring toGta(std::wstring&& text);
		static void destroyGta(std::wstring& text);
	};
}
