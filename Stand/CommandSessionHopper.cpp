#include "CommandSessionHopper.hpp"

#include "AbstractPlayer.hpp"
#include "FiberPool.hpp"
#include "is_session.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "TransitionHelper.hpp"
#include "TransitionState.hpp"

namespace Stand
{
	CommandSessionHopper::CommandSessionHopper(CommandList* parent, CommandSlider* min_players)
		: CommandToggle(parent, LOC("SESSHOP"), { CMDNAME("sessionhopper") }), min_players(min_players)
	{
		instance = this;
	}

	bool CommandSessionHopper::shouldHop() const
	{
		return AbstractPlayer::getPlayerCount() < min_players->value;
	}

	void CommandSessionHopper::onEnable(Click& click)
	{
		registerAsyncTickEventHandler(click, [this]
		{
			HANDLER_CHECK(m_on);
			if (!job_in_progress && !is_session_transition_active(true, false) && shouldHop())
			{
				job_in_progress = true;
				FiberPool::queueJob([this]
				{
					if (ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() == TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER)
					{
						Script::current()->yield(1000);
						TransitionHelper::abortTransition();
						while (is_session_transition_active(true, false))
						{
							Script::current()->yield();
						}
						Script::current()->yield(1000);
					}
					TransitionHelper::doTransition(FIND_PUBLIC, true);
					job_in_progress = false;
				});
			}
			HANDLER_END;
		});
	}
}
