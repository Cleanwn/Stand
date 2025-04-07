#pragma once

#include "CommandToggle.hpp"

#include "ProjectileMgr.hpp"
#include "regular_event.hpp"

namespace Stand
{
	class CommandProjectileMgr : public CommandToggle
	{
	public:
		explicit CommandProjectileMgr(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text = NOLABEL)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), std::move(help_text))
		{
		}

		void onEnable(Click& click) final
		{
			if (++ProjectileMgr::handler_amount == 1)
			{
				reScriptTickEvent::registerHandler(&ProjectileMgr::onTick);
			}
		}

		void onDisable(Click& click) final
		{
			if (--ProjectileMgr::handler_amount == 0)
			{
				reScriptTickEvent::unregisterHandler(&ProjectileMgr::onTick);
			}
		}
	};
}