#pragma once

#include "CommandActionScript.hpp"

#include "Place.hpp"

namespace Stand
{
	class CommandPlace : public CommandActionScript
	{
	private:
		Place* const place;

	public:
		explicit CommandPlace(CommandList* const parent, Place* const place, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL)
			: CommandActionScript(parent, Label(place->name), std::move(command_names), std::move(help_text)), place(place)
		{
			if (place->supportsUsageByOthers()
				&& resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandListPlaces>()->supportsUsageByOthers()
				)
			{
				perm = COMMANDPERM_NEUTRAL;
			}
		}

		void onClickScriptThread(Click& click) final
		{
			if (click.canExecuteChainCommand())
			{
				if (auto pop = place->getClosestPop(click); pop.has_value())
				{
					resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandListPlaces>()->processPop(click, pop.value());
					click.setChainCommandExecuted();
				}
			}
		}
	};
}
