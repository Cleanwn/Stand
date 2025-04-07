#pragma once

#include "CommandPhysical.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandInput : public CommandPhysical
	{
	protected:
		std::string default_value;

	public:
		std::string value;

		inline static bool prefill_value = true;

		explicit CommandInput(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::string&& default_value, std::string&& value, const commandflags_t flags = CMDFLAGS_ACTION_VALUE, const CommandPerm perm = COMMANDPERM_USERONLY);
		explicit CommandInput(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text = NOLABEL, std::string&& default_value = {}, const commandflags_t flags = CMDFLAGS_ACTION_VALUE, const CommandPerm perm = COMMANDPERM_USERONLY);

		std::wstring getCommandSyntax() const override;

		void onClick(Click& click) override;
		void onCommand(Click& click, std::wstring& args) override;
	protected:
		void onCommandApply(Click& click, std::wstring& args);

	public:
		std::string getState() const override;
		std::string getDefaultState() const final;
		void setState(Click& click, const std::string& state) override;
		void applyDefaultState() override;

		void setValue(std::string&& value, ClickType type = CLICK_AUTO);
		void setValueGta(const std::string& value, ClickType type = CLICK_AUTO);
	protected:
		void update_state(ClickType type) const;
		void update_state(const Click& click) const;
	};
#pragma pack(pop)
}
