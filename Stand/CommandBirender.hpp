#pragma once

#include "CommandEspBase.hpp"
#include "CommandListSelect.hpp"

#include "Spinlock.hpp"

namespace Stand
{
	class CommandBirender : public CommandEspBase<CommandListSelect>
	{
	protected:
		Spinlock mtx = {};

	public:
		explicit CommandBirender(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names, CommandColour* const colour, CommandEspTags* const tag_colours = nullptr)
			: CommandEspBase(parent, std::move(menu_name), std::move(command_names), NOLABEL, {
				{0, LOC("DOFF")},
				{1, LOC("RNDR_LWLTCY")},
				{2, LOC("RNDR_LGCY")},
			}, 0)
		{
			this->colour = colour;
			this->tag_colours = tag_colours;
		}

		void onChange(Click& click, long long prev_value) final;

	protected:
		virtual void onTick() = 0;
		virtual void draw(const AbstractRenderer& renderer) = 0;
	};
}
