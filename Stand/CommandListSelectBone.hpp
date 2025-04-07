#pragma once

#include "CommandListSelect.hpp"

#include "PedBoneId.hpp"

namespace Stand
{
	class CommandListSelectBone : public CommandListSelect
	{
	private:
		static std::vector<CommandListActionItemData> getOptions()
		{
			std::vector<CommandListActionItemData> options;

			for (const auto group : major_bone_labels)
			{
				options.emplace_back(CommandListActionItemData(group.first, group.second));
			}

			return options;
		}

	public:
		explicit CommandListSelectBone(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
			: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), getOptions(), PedBoneId::SKEL_Head)
		{
		}
	};
}