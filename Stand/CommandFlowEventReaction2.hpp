#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandFlowEventReaction2 : public CommandToggle
	{
	private:
		const floweventreaction_t reaction;

	public:
		explicit CommandFlowEventReaction2(CommandList* const parent, flowevent_t event, floweventreaction_t reaction, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = {});

		void onChange(Click& click) final;
	};
#pragma pack(pop)
}
