#include "GridItemNotify.hpp"

#include "get_current_time_millis.hpp"
#include "NotifyGrid.hpp"
#include "Renderer.hpp"

namespace Stand
{
	GridItemNotify::GridItemNotify(const std::wstring& text, time_t show_until, time_t flash_until)
		: GridItemText(std::wstring(text), g_notify_grid.notify_width, g_notify_grid.notify_padding, 0,
			((g_notify_grid.invert_flow && !g_notify_grid.next_to_map) ? ALIGN_BOTTOM_LEFT : ALIGN_TOP_CENTRE)
		), show_until(show_until), flash_until(flash_until), frozen(g_notify_grid.isFrozen()), flashing(!IS_DEADLINE_REACHED(flash_until))
	{
	}

	void GridItemNotify::draw()
	{
		if (show_until != 0 && !frozen && IS_DEADLINE_REACHED(show_until))
		{
			g_notify_grid.update();
		}
		/*else if (y < 0 && !g_notify_grid.notifies.empty())
		{
			g_notify_grid.notifies.at(0).show_until = 0;
			g_notify_grid.update();
		}*/
		constexpr int16_t border_width = 3;
		g_renderer.drawRectH(float(x - border_width), (float)y, (float)border_width, (float)height, g_renderer.notifyBorderColour);
		if (!frozen)
		{
			flashing = !IS_DEADLINE_REACHED(flash_until);
		}
		if (!flashing)
		{
			drawBackground(g_renderer.notifyBgColour);
		}
		else
		{
			drawBackground(g_renderer.notifyFlashColour);
		}
		drawText();
	}
}
