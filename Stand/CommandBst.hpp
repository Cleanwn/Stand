#pragma once

#include "CommandToggle.hpp"

#include "is_session.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandBst : public CommandToggle
	{
	private:
		inline static time_t disabled_until = 0;

	public:
		explicit CommandBst(CommandList* const parent)
			: CommandToggle(parent, LOC("BST"), { CMDNAME("bst") })
		{
		}

		static void onTick()
		{
			if (is_session_started_and_transition_finished())
			{
				SOUP_IF_UNLIKELY (g_player_ped.isDead())
				{
					disabled_until = get_current_time_millis() + 1000;
				}
				if (IS_DEADLINE_REACHED(disabled_until))
				{
					*ScriptGlobal(GLOBAL_BST).as<int*>() |= (1 << 0);
				}
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!isInViewport())
			{
				onTickInGameViewport();
			}
			return CommandToggle::onCommand(click, args);
		}

		void onClick(Click& click) final
		{
			if (click.type == CLICK_HOTKEY
				&& !isInViewport()
				)
			{
				onTickInGameViewport();
			}
			onClickRemoveCorrelation(click);
			onClickSoundEffect(click);
			onClickToggleScriptTickEventHandler(click, &onTick);
			if (!is_session_started())
			{
				toggleState(click);
			}
			if (m_on)
			{
				ensureScriptThread(click, []
				{
					if (is_session_started()
						&& (*ScriptGlobal(GLOBAL_BST).as<int*>() & (1 << 0))
						)
					{
						*ScriptGlobal(GLOBAL_BST).as<int*>() |= (1 << 2);
					}
				});
				if (!isInViewport())
				{
					setBstState(false);
				}
			}
		}

	private:
		void setBstState(bool on)
		{
			if (on != m_on)
			{
				setOnIndication(on);
				Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
				updateState(click);
			}
		}

	public:
		void onTickInGameViewport() final
		{
			if (is_session_started())
			{
				setBstState((*ScriptGlobal(GLOBAL_BST).as<int*>() & (1 << 0)));
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
