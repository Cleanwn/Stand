#include "GridItemTabsVertical.hpp"

#include "CommandTab.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	GridItemTabsVertical::GridItemTabsVertical(uint8_t priority, Alignment alignment_relative_to_last)
		: GridItem(GRIDITEM_TABS, int16_t(g_renderer.tabs_width), g_renderer.tabs_height * (int16_t)g_gui.root_list->children.size(), priority, alignment_relative_to_last)
	{
	}

	void GridItemTabsVertical::draw()
	{
		drawBackgroundEffects();

		int8_t root_draw_cursor = 0;
		auto cursorPosH = g_renderer.getCursorPosH(true);
		int focus_y;
		for (const auto& tab : g_gui.root_list->children)
		{
			const bool focused = (g_menu_grid.root_cursor == root_draw_cursor);
			const int tab_y = (y + (g_renderer.tabs_height * root_draw_cursor));
			if (focused)
			{
				focus_y = tab_y;
			}
			if (g_renderer.drawRectH((float)x, (float)tab_y, (float)width, (float)g_renderer.tabs_height, (focused ? g_renderer.getFocusRectColour() : g_renderer.getBgRectColour()), cursorPosH) && g_renderer.mouse_squeaks && g_gui.mouse_mode == MouseMode::NAVIGATE)
			{
				if (g_gui.root_cursor != root_draw_cursor)
				{
					g_gui.last_input_type = INPUTTYPE_MOUSE_MOVE;
					auto prev_focus = g_gui.getCurrentMenuFocus()->getWeakRef();
					g_gui.root_cursor = root_draw_cursor;
					g_gui.onRootCursorUpdate(TC_RENDERER);
					g_gui.updateFocus(TC_RENDERER, TELEPORT, prev_focus);
				}
			}
			auto text_x = float(x);
			switch (g_renderer.tabs_alignment)
			{
			case ALIGN_TOP_LEFT:
				text_x += 5.0f;
				break;

			case ALIGN_TOP_CENTRE:
				text_x += (float(width) * 0.5f);
				break;

			case ALIGN_TOP_RIGHT:
				text_x += float(width - 5);
				break;
			}
			std::wstring text = tab->as<CommandTab>()->render_name;
			g_renderer.trimTextH(text, g_renderer.tabs_text.scale, float(g_renderer.tabs_width - 10));
			g_renderer.drawTextH(text_x, float(tab_y), text.c_str(), (focused ? g_renderer.getFocusTextColour() : g_renderer.getBgTextColour()), g_renderer.tabs_text, g_renderer.tabs_alignment);
			++root_draw_cursor;
		}

		if (g_renderer.getCursorBorderWidth() != 0)
		{
			g_renderer.drawBorderH((float)x, (float)focus_y, (float)width, (float)g_renderer.tabs_height, g_renderer.getCursorBorderWidth(), g_renderer.getCursorBorderColour(), g_renderer.getCursorBorderRounded());
		}
	}
}
