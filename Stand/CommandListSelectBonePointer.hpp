#pragma once

#include "CommandListSelectBone.hpp"

namespace Stand
{
	class CommandListSelectBonePointer : public CommandListSelectBone
	{
	private:
		PedBoneId* const ptr;

	public:
		explicit CommandListSelectBonePointer(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, PedBoneId* const ptr)
			: CommandListSelectBone(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), ptr(ptr)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			*ptr = (PedBoneId)this->value;
		}
	};
}