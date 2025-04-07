#include "CommandShowArraySynchProgress.hpp"

#include <fmt/xchar.h>

#include "AbstractPlayer.hpp"
#include "get_current_time_millis.hpp"
#include "net_array.hpp"
#include "ScriptGlobal.hpp"
#include "script_handler.hpp"
#include "script_thread.hpp"
#include "TransitionState.hpp"
#include "regular_event.hpp"
#include "Renderer.hpp"

namespace Stand
{
	static bool arrays_completed = false;
	static uint32_t num_fm_array_handlers = 0;
	static uint32_t num_updated_fm_array_handlers;
	static time_t show_array_synch_progress_until = 0ll;

	CommandShowArraySynchProgress::CommandShowArraySynchProgress(CommandList* const parent)
		: CommandToggle(parent, LOC("SHWARRSYNC"))
	{
	}

	void CommandShowArraySynchProgress::onEnable(Click& click)
	{
		show_array_synch_progress_until = (click.isAuto() ? 1ll : (get_current_time_millis() + 3000ll));

		reScriptTickEvent::registerHandler(&onTick);
		rePresentEvent::registerHandler(&onPresent);
	}

	void CommandShowArraySynchProgress::onDisable(Click& click)
	{
		reScriptTickEvent::unregisterHandler(&onTick);
		rePresentEvent::unregisterHandler(&onPresent);
	}

	void CommandShowArraySynchProgress::onTick()
	{
		if (!arrays_completed
			|| ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() != TRANSITION_STATE_TERMINATE_MAINTRANSITION
			)
		{
			if (auto fm = GtaThread::fromHash(ATSTRINGHASH("freemode")))
			{
				if (auto netcomp = fm->getNetComponent())
				{
					if (num_fm_array_handlers = netcomp->m_NumHostBroadcastData)
					{
						if (/*AbstractPlayer::getPlayerCount() == 1 ||*/ netcomp->amHost())
						{
							arrays_completed = true;
						}
						else
						{
							num_updated_fm_array_handlers = 0;
							for (uint32_t i = 0; i != num_fm_array_handlers; ++i)
							{
								auto hostBroadcastDataHandler = netcomp->GetHostBroadcastDataHandler(i);
								if (hostBroadcastDataHandler->m_bHasReceivedAnUpdate)
								{
									++num_updated_fm_array_handlers;
								}
							}
							arrays_completed = (num_fm_array_handlers == num_updated_fm_array_handlers);
						}
					}
				}
				else
				{
					num_fm_array_handlers = 0;
				}
			}
			else
			{
				num_fm_array_handlers = 0;
			}
		}
	}

	void CommandShowArraySynchProgress::onPresent()
	{
		if (num_fm_array_handlers)
		{
			if (arrays_completed)
			{
				SOUP_IF_UNLIKELY (show_array_synch_progress_until == 0)
				{
					show_array_synch_progress_until = get_current_time_millis() + 3000ll;
				}
				else if (IS_DEADLINE_REACHED(show_array_synch_progress_until))
				{
					return;
				}
			}
			else
			{
				show_array_synch_progress_until = 0;
			}
			g_renderer.drawCentredTextThisFrame(LANG_FMT_W("SHWARRSYNC_T", num_updated_fm_array_handlers, num_fm_array_handlers, (int)((float)num_updated_fm_array_handlers / num_fm_array_handlers * 100)));
		}
	}
}
