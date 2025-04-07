#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandHoistTag : public CommandToggle
	{
	private:
		const playerflag_t id;

	public:
		explicit CommandHoistTag(CommandList* const parent, playerflag_t id, Label&& name)
			: CommandToggle(parent, std::move(name)), id(id)
		{
		}

		void onEnable(Click& click) final
		{
			PlayerListSort::hoisted_flags.emplace_back(id);
			PlayerListSort::update();
		}

		void onDisable(Click& click) final
		{
			PlayerListSort::hoisted_flags.erase(std::find(PlayerListSort::hoisted_flags.begin(), PlayerListSort::hoisted_flags.end(), id));
			PlayerListSort::update();
		}
	};
}
