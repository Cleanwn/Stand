#include "GridItemScrollbar.hpp"

#include <algorithm>

#include "CommandList.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	GridItemScrollbar::GridItemScrollbar(CommandList* view, int16_t width, int16_t height, uint8_t priority)
		: GridItem(GRIDITEM_SCROLLBAR, width, height, priority, ALIGN_TOP_RIGHT), view(view)
	{
		update();
	}

	void GridItemScrollbar::update()
	{
		if (height != 0)
		{
			auto num_children = view->countVisibleChildren();
			if (num_children != 0)
			{
				thumb_height = std::max(1i16, int16_t(float(height) / float(num_children) * float(std::min(num_children, size_t(g_gui.getCommandsOnScreenLimit())))));
				thumb_offset = int16_t(float(view->m_offset) / float(num_children - g_gui.getCommandsOnScreenLimit()) * float(height - thumb_height));
			}
		}
	}

	void GridItemScrollbar::draw()
	{
		drawBackgroundEffects();
		auto cursorPosH = g_renderer.getCursorPosH(true);
		if (g_renderer.drawRectH(x, y, width, height, g_renderer.getBgRectColour(), g_renderer.mouse_squeaks ? g_renderer.cursorPosWhenClickStartedH : cursorPosH)
			&& g_renderer.mouse_squeaks && cursorPosH.y > y && (y + height) >= cursorPosH.y)
		{
			view->approachCursor(cursor_t((float(cursorPosH.y - y) / float(height)) * float(view->countVisibleChildren())));
		}
		g_renderer.drawRectH((float)x, float(y + thumb_offset), (float)width, (float)thumb_height, g_renderer.getFocusRectColour());
	}
}
