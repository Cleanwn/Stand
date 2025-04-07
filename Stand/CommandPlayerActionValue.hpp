#pragma once

#include "CommandPlayerAction.hpp"

namespace Stand
{
	class CommandPlayerActionValue : public CommandPlayerAction
	{
	private:
		const long long min_value;
		const long long max_value;

	protected:
		explicit CommandPlayerActionValue(CommandList* const parent, long long min_value, long long max_value, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text = NOLABEL, CommandPerm perm = COMMANDPERM_USERONLY);

	public:
		std::wstring getCommandSyntax() const final;

		void onClick(Click& click) final;

		void onCommand(Click& click, std::wstring& args) final;

	protected:
		void onCommandRangeCheck(Click& click, long long value);
		
		virtual void onCommandWithValue(Click& click, long long value) = 0;
	};
}
