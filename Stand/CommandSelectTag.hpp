#pragma once

#include "CommandListSelect.hpp"

#include "player_tags.hpp"

namespace Stand
{
	class CommandSelectTag : public CommandListSelect
	{
	private:
		const playerflag_t flag;

	public:
		explicit CommandSelectTag(CommandList* const parent, Label&& menu_name, playerflag_t flag);
		
		void onChange(Click& click, long long prev_value) final;
	};
}
