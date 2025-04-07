#pragma once

#include "CommandListSelect.hpp"

#include "MenuGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandAddressSeparator : public CommandListSelect
	{
	private:
		inline static const wchar_t* values[] = {
			L" > ",
			L">",
			L" / ",
			L"/",
			L" : ",
			L":",
			L" \\ ",
			L"\\",
		};

	public:
		explicit CommandAddressSeparator(CommandList* const parent)
			: CommandListSelect(parent, LOC("ADDRSEP"), {}, NOLABEL, {
				{0, LIT("Stand > Online")},
				{1, LIT("Stand>Online")},
				{2, LIT("Stand / Online")},
				{3, LIT("Stand/Online")},
				{4, LIT("Stand : Online")},
				{5, LIT("Stand:Online")},
				{6, LIT("Stand \\ Online")},
				{7, LIT("Stand\\Online")},
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_renderer.address_separator = values[value];
			});
		}
	};
}
