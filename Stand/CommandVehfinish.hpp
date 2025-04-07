#pragma once

#include "CommandListSelect.hpp"

#include <soup/macros.hpp>

#include "CommandVehcolour.hpp"

namespace Stand
{
	class CommandVehfinish : public CommandListSelect
	{
	private:
		CommandVehcolour* const colour;

	public:
		explicit CommandVehfinish(CommandList* const parent, CommandVehcolour* colour, Label&& menu_name, std::vector<CommandName>&& command_names)
			: CommandListSelect(parent, std::move(menu_name), std::move(command_names), NOLABEL, {
				{0, LOC("VEHFIN0")},
				{1, LOC("VEHFIN1")},
				{2, LOC("VEHFIN2")},
				{3, LOC("VEHFIN3")},
				{4, LOC("VEHFIN4")},
				{5, LOC("VEHFIN5")},
				{6, LOC("VEHFIN6")},
			}, 0, CMDFLAGS_LIST_SELECT & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS), colour(colour)
		{
			colour->finish_cmd = this;
		}
		
		~CommandVehfinish() override
		{
			colour->finish_cmd = nullptr;
		}

		void onChange(Click& click, long long prev_value) final
		{
			click.ensureScriptThread([this](Click& click)
			{
				colour->onChange(click);
			});
		}
	};
}
