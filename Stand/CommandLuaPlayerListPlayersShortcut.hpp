#pragma once

#include "CommandPlayerListPlayersShortcut.hpp"
#include "CommandLua.hpp"

namespace Stand
{
	class CommandLuaPlayerListPlayersShortcut : public CommandLua<CommandPlayerListPlayersShortcut>
	{
	private:
		const std::string command_name;

	public:
		explicit CommandLuaPlayerListPlayersShortcut(CommandList* const parent, CommandLuaScript* const script, Label&& menu_name, std::string&& command_name, bool single_only)
			: CommandLua(parent, script, std::move(menu_name), single_only), command_name(std::move(command_name))
		{
		}

		[[nodiscard]] std::string getChild(AbstractPlayer& blamed_on, const std::string& target) const final
		{
			return fmt::format("as {} {} {}", blamed_on.getName(), command_name, target);
		}
	};
}
