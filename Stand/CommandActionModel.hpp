#pragma once

#include "CommandAction.hpp"

#include "AbstractModel.hpp"

namespace Stand
{
	class CommandActionModel : public CommandAction
	{
	protected:
		explicit CommandActionModel(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text = NOLABEL, const commandflags_t flags = 0, const CommandPerm perm = COMMANDPERM_USERONLY);

	public:
		std::wstring getCommandSyntax() const final;

		void onCommand(Click& click, std::wstring& args) override;
		
	protected:
		virtual bool spawnIfValid(const AbstractModel& model, Click& click, const Label& name) const = 0;
		[[nodiscard]] virtual hash_t getInvalidModelMessage() const;
	};
}
