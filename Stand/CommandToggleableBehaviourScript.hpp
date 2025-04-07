#pragma once

#include "CommandToggle.hpp"

#include "tbToggleableBehaviourScript.hpp"

namespace Stand
{
	class CommandToggleableBehaviourScript : public CommandToggle
	{
	private:
		tbToggleableBehaviourScript* const toggleable_behaviour;

	public:
		explicit CommandToggleableBehaviourScript(CommandList* const parent, tbToggleableBehaviourScript* const toggleable_behaviour, Label&& menu_name, std::vector<CommandName>&& command_names)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names)), toggleable_behaviour(toggleable_behaviour)
		{
		}

		void onEnable(Click& click) final
		{
			toggleable_behaviour->enable();
		}

		void onDisable(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				toggleable_behaviour->disable();
			});
		}
	};
}
