#include "CommandPlayerTimeout.hpp"

#include "CommandPlayer.hpp"
#include "get_current_time_millis.hpp"
#include "Hooking.hpp"
#include "PlayerProviderSingle.hpp"

namespace Stand
{
	CommandPlayerTimeout::CommandPlayerTimeout(CommandList* const parent)
		: CommandPlayerToggle(parent, LOC("TIMEOUT"), CMDNAMES_OBF("timeout"), LOC("TIMEOUT_H"))
	{
	}

	void CommandPlayerTimeout::onEnable(Click& click)
	{
		if (PC_PTR_SINGLE->isSelf())
		{
			if (click.type != CLICK_AUTO)
			{
				click.setResponse(LOC("CMDOTH"));
			}
			setOnIndication(false);
			return;
		}
		g_hooking.putInTimeout(PP_PTR_SINGLE->p);
	}

	void CommandPlayerTimeout::onDisable(Click& click)
	{
		if (click.isMenu()
			&& GET_MILLIS_SINCE(PC_PTR_SINGLE->last_attack) < 2000
			)
		{
			setOnIndication(true);
			showWarning(click, LANG_GET_W("GENWARN"), WARNING_COMMAND_BOUND, [this](ThreadContext tc)
			{
				setOnIndication(false);
				g_hooking.player_timeouts[PP_PTR_SINGLE->p] = 0;
			});
			return;
		}
		g_hooking.player_timeouts[PP_PTR_SINGLE->p] = 0;
	}
}
