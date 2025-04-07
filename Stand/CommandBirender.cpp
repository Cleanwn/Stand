#include "CommandBirender.hpp"

#include "AbstractRenderer.hpp"
#include "CommandToggleRegularSp.hpp"
#include "Exceptional.hpp"
#include "Renderer.hpp"
#include "tbScreenshotMode.hpp"

namespace Stand
{
	void CommandBirender::onChange(Click& click, long long prev_value)
	{
		if (value == 0)
		{
			return;
		}
		if (!CommandToggleRegularSp::checkPermissions())
		{
			click.setResponse(LOC("CMDFULLSP"));
			auto click_ = click.deriveAuto();
			setValue(click_, 0);
			return;
		}
		if (value == 1)
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (value != 1)
				{
					return false;
				}
				if (CommandToggleRegularSp::checkPermissions()
					&& !g_tb_screenshot_mode.isEnabled()
					)
				{
					EXCEPTIONAL_LOCK(mtx)
					onTick();
					EXCEPTIONAL_UNLOCK(mtx)
				}
				return true;
			});
			registerPresentEventHandler([this]
			{
				if (value != 1)
				{
					return false;
				}
				if (CommandToggleRegularSp::checkPermissions()
					&& g_renderer.doesGameplayStateAllowScriptExecution()
					&& !g_tb_screenshot_mode.isEnabled()
					)
				{
					EXCEPTIONAL_LOCK(mtx)
					draw(g_dx_render);
					EXCEPTIONAL_QUIET_UNLOCK(mtx)
				}
				return true;
			});
		}
		else if (value == 2)
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (value != 2)
				{
					return false;
				}
				if (CommandToggleRegularSp::checkPermissions()
					&& !g_tb_screenshot_mode.isEnabled()
					)
				{
					EXCEPTIONAL_LOCK(mtx)
					onTick();
					draw(g_native_render);
					EXCEPTIONAL_UNLOCK(mtx)
				}
				return true;
			});
		}
	}
}
