#pragma once

#include "CommandListNameshare.hpp"

#include "CommandListActionItemData.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandListAction : public CommandListNameshare
	{
	private:
		commandflags_t item_flags;

	public:
		std::vector<CommandListActionItemData> options;

		explicit CommandListAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandListActionItemData>&& options, const commandflags_t flags = CMDFLAGS_LIST_ACTION, const commandflags_t item_flags = CMDFLAGS_LIST_ACTION, const CommandType type = COMMAND_LIST_ACTION);

		void setOptions(std::vector<CommandListActionItemData>&& options);
		void setOptions(const std::vector<CommandListActionItemData>& options);
	private:
		[[nodiscard]] CommandList* getCategory(const Label& name);
		void populate();
	public:
		void updateCommandNamesOfChildren();

		virtual void onItemClick(Click& click, CommandListActionItem* item);

		[[nodiscard]] const CommandListActionItemData* getValueData(long long value) const;
		[[nodiscard]] CommandListActionItem* getValueCommand(long long value) const;

		[[nodiscard]] Label getValueMenuName(long long value) const;
	};
#pragma pack(pop)
}
