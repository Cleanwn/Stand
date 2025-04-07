#include "tbScreenshotMode.hpp"

#include "ExecCtx.hpp"
#include "hud_component.hpp"
#include "natives.hpp"
#include "NotifyGrid.hpp"
#include "pointers.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	void tbScreenshotMode::onEnable()
	{
		tbToggleableBehaviourScript::onEnable();
		g_notify_grid.freeze();
	}

	void tbScreenshotMode::onDisable()
	{
		g_notify_grid.unfreeze();
		ExecCtx::get().ensureScript([]
		{
			HUD::MP_TEXT_CHAT_DISABLE(FALSE);
			ScriptGlobal(GLOBAL_TIMERS + GLOBAL_TIMERS_BI_OFFSET).set<int>(0);
		});
	}

	void tbScreenshotMode::onTick()
	{
		*pointers::hide_hud_and_radar_this_frame = true;
		HUD::THEFEED_HIDE_THIS_FRAME();
		HUD::HIDE_HUD_COMPONENT_THIS_FRAME(HUD_HELP_TEXT);
		HUD::MP_TEXT_CHAT_DISABLE(TRUE);
		ScriptGlobal(GLOBAL_TIMERS + GLOBAL_TIMERS_BI_OFFSET).set<int>(-10);
	}
}
