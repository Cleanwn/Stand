#include "GridItemTabsHorizontal.hpp"

#include "CommandTab.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	GridItemTabsHorizontal::GridItemTabsHorizontal(uint8_t priority, Alignment alignment_relative_to_last, GridItem* force_alignment_to)
		: GridItem(GRIDITEM_TABS, int16_t(g_renderer.tabs_width * g_gui.root_list->children.size()), g_renderer.tabs_height, priority, alignment_relative_to_last, force_alignment_to)
	{
	}

	void GridItemTabsHorizontal::draw()
	{
		drawBackgroundEffects();

		auto cursorPosH = g_renderer.getCursorPosH(true);
		int8_t root_draw_cursor = 0;
		auto x = this->x;
		int focus_x;
		for (const auto& tab : g_gui.root_list->children)
		{
			const bool focused = (g_gui.root_cursor == root_draw_cursor);
			if (focused)
			{
				focus_x = x;
			}
			if (g_renderer.drawRectH(x, y, g_renderer.tabs_width, g_renderer.tabs_height, (focused ? g_renderer.getFocusRectColour() : g_renderer.getBgRectColour()), cursorPosH) && g_renderer.mouse_squeaks && g_gui.mouse_mode == MouseMode::NAVIGATE)
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
				text_x += 5;
				break;

			case ALIGN_TOP_CENTRE:
				text_x += (float(g_renderer.tabs_width) * 0.5f);
				break;

			case ALIGN_TOP_RIGHT:
				text_x += float(g_renderer.tabs_width - 5);
				break;
			}
			std::wstring text = tab->as<CommandTab>()->render_name;
			g_renderer.trimTextH(text, g_renderer.tabs_text.scale, float(g_renderer.tabs_width - 10));
			g_renderer.drawTextH(text_x, y, text.c_str(), (focused ? g_renderer.getFocusTextColour() : g_renderer.getBgTextColour()), g_renderer.tabs_text, g_renderer.tabs_alignment);
			x += g_renderer.tabs_width;
			++root_draw_cursor;
		}

		if (g_renderer.getCursorBorderWidth() != 0)
		{
			g_renderer.drawBorderH((float)focus_x, (float)y, (float)g_renderer.tabs_width, (float)g_renderer.tabs_height, g_renderer.getCursorBorderWidth(), g_renderer.getCursorBorderColour(), g_renderer.getCursorBorderRounded());
		}
	}
}
