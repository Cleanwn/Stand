#pragma once

#include "CommandListSelect.hpp"

#include "drawDebugText.hpp"

namespace Stand
{
	class CommandInfoAlign : public CommandListSelect
	{
	public:
		explicit CommandInfoAlign(CommandList* const parent)
			: CommandListSelect(parent, LOC("ALGN"), CMDNAMES("infoalign"), NOLABEL, {
				{ALIGN_TOP_LEFT, LOC("ALIGN_TL"), CMDNAMES("topleft")},
				{ALIGN_TOP_CENTRE, LOC("ALIGN_TC"), CMDNAMES("topcentre", "topcenter")},
				{ALIGN_TOP_RIGHT, LOC("ALIGN_TR"), CMDNAMES("topright")},
				{ALIGN_CENTRE_LEFT, LOC("ALIGN_CL"), CMDNAMES("centreleft", "centerleft")},
				{ALIGN_CENTRE, LOC("ALIGN_CC"), CMDNAMES("centre", "center")},
				{ALIGN_CENTRE_RIGHT, LOC("ALIGN_CR"), CMDNAMES("centreright", "centerright", "centeright")},
				{ALIGN_BOTTOM_LEFT, LOC("ALIGN_BL"), CMDNAMES("bottomleft")},
				{ALIGN_BOTTOM_CENTRE, LOC("ALIGN_BC"), CMDNAMES("bottomcentre", "bottomcenter")},
				{ALIGN_BOTTOM_RIGHT, LOC("ALIGN_BR"), CMDNAMES("bottomright")},
			}, ALIGN_TOP_RIGHT)
		{
		}

		void onFocus(ThreadContext thread_context, Direction momentum) final
		{
			g_force_debug_text = true;
		}

		void onBlur(ThreadContext thread_context, Direction momentum) final
		{
			g_force_debug_text = false;
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				g_renderer.info_alignment = (Alignment)value;
			}
		}
	};
}
