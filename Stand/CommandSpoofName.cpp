#include "CommandSpoofName.hpp"

#include "AbstractEntity.hpp"
#include "apply_spoofed_name.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "is_session.hpp"
#include "pointers.hpp"
#include "rlGamerInfo.hpp"
#include "Util.hpp"

namespace Stand
{
	void CommandSpoofName::onEnable(Click& click)
	{
		if (g_gui.root_state != GUI_MINIMAL
			&& !click.isBasicEdition()
			)
		{
			return onChangeImplUnavailable(click);
		}
		g_hooking.spoof_name = true;
		on_name_spoofing_update();
		registerAsyncTickEventHandler(click, [=]()
		{
			if (g_hooking.spoof_name)
			{
				apply_spoofed_name(g_hooking.spoofed_name.c_str());
			}
			return g_hooking.spoof_name;
		});
		if (click.type != CLICK_BULK)
		{
			//click.setSpoofUpdate();
		}
	}

	void CommandSpoofName::onDisable(Click& click)
	{
		g_hooking.spoof_name = false;
		apply_spoofed_name(pointers::rlPresence_GamerPresences_0_GamerInfo->name);
		on_name_spoofing_update();
		if (click.type != CLICK_BULK)
		{
			//click.setSpoofUpdate();
		}
	}
}
