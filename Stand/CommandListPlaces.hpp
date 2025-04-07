#pragma once

#include "CommandList.hpp"

#include "Place.hpp"

namespace Stand
{
	class CommandListPlaces : public CommandList
	{
	public:
		explicit CommandListPlaces(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {})
			: CommandList(parent, std::move(menu_name), std::move(command_names), NOLABEL, CMDFLAGS_LIST, COMMAND_LIST_CUSTOM_SPECIAL_MEANING)
		{
		}

		void populate(bool for_teleport);

		[[nodiscard]] virtual bool supportsUsageByOthers() const noexcept = 0;
		[[nodiscard]] virtual std::vector<CommandName> decorateCommandNames(const std::vector<CommandName>& command_names) const = 0;
		virtual void processPop(Click& click, const PointOfPresence& pop) const = 0;
	};
}
