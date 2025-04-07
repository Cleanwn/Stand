#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandSavePos : public CommandAction
	{
	public:
		explicit CommandSavePos(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names);

		[[nodiscard]] std::wstring getCommandSyntax() const final;

		void onCommand(Click& click, std::wstring& args) override;
	protected:
		[[nodiscard]] virtual std::string getPos() const = 0;
	};
}
