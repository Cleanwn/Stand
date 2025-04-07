#include "GridItemAddressbar.hpp"

#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"
#include "RootNameMgr.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	GridItemAddressbar::GridItemAddressbar(int16_t width)
		: GridItemPrimaryText(width, g_renderer.addressbar_height)
	{
	}

	void GridItemAddressbar::onPositioned()
	{
		auto text_width = width - 10;
		Command* command = nullptr;
		if (g_gui.isWarningActive())
		{
			command = g_gui.getCurrentMenuFocus();
		}
		else if (g_gui.isPromptActive())
		{
			command = g_gui.root_list.get();
		}
		else
		{
			CommandList* list = g_gui.getCurrentUiList();
			if (list == nullptr)
			{
				return;
			}
			if (g_gui.show_cursor_pos)
			{
				cursor_text = list->getCursorText();
				auto cursor_text_width = int16_t(g_renderer.getTextWidth(cursor_text, g_menu_grid.addressbar_text.scale));
				cursor_text_width += 5;
				cursor_text_x = (x + width) - cursor_text_width;
				text_width -= cursor_text_width;
			}
			command = list;
		}
		text = g_renderer.getAddressText(command, float(text_width));
	}

	void GridItemAddressbar::draw()
	{
		GridItemPrimaryText::draw(g_menu_grid.addressbar_text);
		if (!cursor_text.empty())
		{
			g_renderer.drawTextH(cursor_text_x, y, cursor_text.c_str(), g_renderer.getFocusTextColour(), g_menu_grid.addressbar_text);
		}
	}
}
