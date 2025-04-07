#pragma once

#include "CommandLua.hpp"
#include "CommandColour.hpp"

namespace Stand
{
	class CommandLuaColour : public CommandLuaPhysical<CommandColour>
	{
	private:
		const int on_change_ref;

	public:
		explicit CommandLuaColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const DirectX::SimpleMath::Color& default_rgba, bool transparency, CommandLuaScript* const script, const int on_change_ref);

		void onChange(Click& click) final;
	};
}
