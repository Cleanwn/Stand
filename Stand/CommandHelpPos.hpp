#pragma once

#include "CommandListSelect.hpp"

#include "MenuGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	class CommandHelpPos : public CommandListSelect
	{
	public:
		explicit CommandHelpPos(CommandList* const parent)
			: CommandListSelect(parent, LOC("POS"), { CMDNAME("commandinfotextposition"), CMDNAME("cmdinfotextposition") }, LOC("IPSUM"), {
				{ALIGN_TOP_LEFT, LOC("RPOS_L"), CMDNAMES("left")},
				{ALIGN_BOTTOM_LEFT, LOC("RPOS_B"), CMDNAMES("bottom")},
				{ALIGN_TOP_RIGHT, LOC("RPOS_R"), CMDNAMES("right")},
				{ALIGN_BOTTOM_RIGHT, LOC("BLWTBS"), CMDNAMES("belowtabs")},
			}, ALIGN_TOP_LEFT)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			g_menu_grid.updateSetting([this]
			{
				g_renderer.info_text_alignment = (Alignment)value;
			});
		}
	};
}
