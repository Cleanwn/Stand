#pragma once

#include "CommandToggle.hpp"
#include "CommandWithOnTickFocused.hpp"

#include "DrawUtil3d.hpp"
#include "FiberPool.hpp"
#include "ipls.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "Script.hpp"
#include "TpUtil.hpp"

namespace Stand
{
	class CommandIpl : public CommandWithOnTickFocused<CommandToggle>
	{
	private:
		CommandToggleNoCorrelation* const tp_toggle;
		const IplItem* const item;

	public:
		explicit CommandIpl(CommandList* const parent, CommandToggleNoCorrelation* tp_toggle, const IplItem* const item, Label&& help_text)
			: CommandWithOnTickFocused(parent, Label(item->menu_name), std::vector<CommandName>(item->command_names), std::move(help_text)), tp_toggle(tp_toggle), item(item)
		{
		}

		void onTickFocused() final
		{
			DrawUtil3d::draw_line_and_ar_beacon(item->coords);
		}

		void doTp()
		{
			if (tp_toggle->m_on)
			{
				FiberPool::queueJob([this]
				{
					Script::current()->yield();
					TpUtil::teleport(item->coords);
				});
			}
		}

		void onEnable(Click& click) final
		{
			if (item->togglable_behaviour != nullptr)
			{
				item->togglable_behaviour->enable();
				if (click.type != CLICK_BULK)
				{
					doTp();
				}
			}
			else
			{
				ensureScriptThread(click, [this, click]
				{
					if (item->special_behaviour >= IPLSPEC_MP_WORLD_STATE)
					{
						auto is_bulk = click.isBulk();
						FiberPool::queueJob([is_bulk]
						{
							if (is_bulk)
							{
								// Prevent nasty stuff if starting the game with such an IPL enabled in state
								Script::current()->yield(1000);
							}

							WorldState::setOnline(TRUE);
						});
					}
					item->data.enable();
					if (click.type != CLICK_BULK)
					{
						doTp();
					}
					registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [this]()
					{
						if (m_on)
						{
							if (item->special_behaviour == IPLSPEC_SET_UNBLOCK_GLOBAL && is_session_started())
							{
								*ScriptGlobal(MPGlobalsAmbience_g_bDoorsInited).as<BOOL*>() = false;
							}
							item->data.tick();
							return true;
						}
						item->data.disable();
						return false;
					});
				});
			}
		}

		void onDisable(Click& click) final
		{
			if (item->togglable_behaviour != nullptr)
			{
				item->togglable_behaviour->disable();
			}
		}
	};
}
