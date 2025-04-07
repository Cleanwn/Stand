#pragma once

#include "CommandActionModel.hpp"

#include "Util.hpp"

namespace Stand
{
	class CommandModelBlacklistAdd : public CommandActionModel
	{
	public:
		explicit CommandModelBlacklistAdd(CommandList* parent)
			: CommandActionModel(parent, LOC("ADDMDL"), { CMDNAME("addblacklistmodel") })
		{
		}

		bool spawnIfValid(const AbstractModel& model, Click& click, const Label& name) const final
		{
			if (model.isValidModel())
			{
				parent->as<CommandListUserkv>()->addItem(click.thread_context, Util::to_padded_hex_string(model), {});
				return true;
			}
			return false;
		}
	};
}
