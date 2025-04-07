#include "Tutorial.hpp"

#include "FiberPool.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"
#include "Script.hpp"
#include "TutorialGrid.hpp"

namespace Stand
{
	static bool og_show_syntax;
	static Direction og_tabs_pos;
	static uint16_t origin_x_offset;

	void Tutorial::start()
	{
		g_gui.inputStop();

		setState(TUT_OPEN);

		// Hide command syntax for the navigation tutorial
		og_show_syntax = g_gui.show_syntax;
		g_gui.show_syntax = false;

		// Hide tabs until user understands basic navigation
		og_tabs_pos = g_renderer.tabs_pos;
		if (g_renderer.tabs_pos != NONE)
		{
			g_renderer.tabs_pos = NONE;
			origin_x_offset = (g_renderer.tabs_width + g_menu_grid.spacer_size);
		}
		else
		{
			origin_x_offset = 0;
		}
		g_gui.updateTabsIgnoreContextMenu(); // always doing this to force view into root list
		g_menu_grid.origin.x += origin_x_offset;

		g_gui.opened = false;

		g_menu_grid.update();
	}

	void Tutorial::setState(uint8_t state)
	{
		Tutorial::state = state;
		update();

		if (state == TUT_ROOT)
		{
			FiberPool::queueJob([]
			{
				Script::current()->yield(1500);
				if (Tutorial::state == TUT_ROOT)
				{
					Tutorial::setState(TUT_ROOT + 1);
				}
			});
		}
		else if (state == TUT_ROOT_DOWN)
		{
			if (og_tabs_pos == NONE)
			{
				g_renderer.tabs_pos = LEFT;
			}
			else
			{
				g_renderer.tabs_pos = og_tabs_pos;
			}
			g_menu_grid.origin.x -= origin_x_offset;
			g_gui.updateTabsIgnoreContextMenu();
			g_menu_grid.update();
		}
		else if (state == TUT_DONE)
		{
			if (!g_gui.user_understands_navigation)
			{
				g_gui.show_syntax = og_show_syntax;

				if (og_tabs_pos == NONE)
				{
					g_renderer.tabs_pos = NONE;
					g_gui.updateTabsIgnoreContextMenu();
					g_menu_grid.update();
				}

				g_gui.user_understands_navigation = true;
				g_gui.saveTutorialFlags();
			}
		}
		else if (state == TUT_PROFILES)
		{
			FiberPool::queueJob([]
			{
				Script::current()->yield(20'000);
				if (Tutorial::state == TUT_PROFILES)
				{
					g_gui.setProfilesTutorialDone();
				}
			});
		}
	}

	void Tutorial::fastForward()
	{
		if (state < TUT_ROOT_DOWN)
		{
			setState(TUT_ROOT_DOWN);
		}
		if (state < TUT_DONE)
		{
			setState(TUT_DONE);
		}
	}

	void Tutorial::update()
	{
		g_tutorial_grid.updateNow();
	}
}
