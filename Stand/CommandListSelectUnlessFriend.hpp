#pragma once

#include "CommandListSelect.hpp"

#include "ReactionExemption.hpp"

namespace Stand
{
	class CommandListSelectUnlessFriend : public CommandListSelect
	{
	private:
		ReactionExemption* const option;

	public:
		explicit CommandListSelectUnlessFriend(CommandList* parent, ReactionExemption* const option, Label&& menu_name = LOC("PTX_NET_R_B"), Label&& help_text = NOLABEL)
			: CommandListSelect(parent, std::move(menu_name), {}, std::move(help_text), std::vector<CommandListActionItemData>{
				{ ReactionExemption::OFF, LOC("DOFF") },
				{ ReactionExemption::ON, LOC("ON") },
				{ ReactionExemption::ON_UNLESS_FRIEND, LOC("PS_S") }
			}, ReactionExemption::OFF), option(option)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			option->value = (uint8_t)this->value;
		}
	};
}