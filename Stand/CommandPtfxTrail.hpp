#pragma once

#include "CommandToggle.hpp"

#include "PedBoneId.hpp"

namespace Stand
{
	class CommandPtfxTrail : public CommandToggle
	{
	public:
		PedBoneId bone;
		time_t last_applied;

		explicit CommandPtfxTrail(CommandList* const parent, PedBoneId bone, Label&& bone_name, std::vector<CommandName>&& command_names, bool default_on)
			: CommandToggle(parent, std::move(bone_name), std::move(command_names), NOLABEL, default_on), bone(bone)
		{
		}
	};
}