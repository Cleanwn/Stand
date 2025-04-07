#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandFlowEventReaction4 : public CommandListSelect
	{
	private:
		const floweventreaction_t reaction;

	public:
		explicit CommandFlowEventReaction4(CommandList* const parent, flowevent_t event, floweventreaction_t reaction, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = {});

		void onChange(Click& click, long long prev_value) final;
	};
#pragma pack(pop)
}
