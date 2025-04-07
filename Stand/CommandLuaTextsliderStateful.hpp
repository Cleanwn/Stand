#pragma once

#include "CommandLua.hpp"
#include "CommandTextsliderStateful.hpp"

namespace Stand
{
	class CommandLuaTextsliderStateful : public CommandLuaPhysical<CommandTextsliderStateful>
	{
	private:
		const int on_click_ref;

	public:
		explicit CommandLuaTextsliderStateful(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, std::vector<CommandTextsliderData>&& options, const int on_click_ref);

		void onClick(Click& click) final;
	};
}
