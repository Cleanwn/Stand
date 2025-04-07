#pragma once

#include "CommandTextslider.hpp"

namespace Stand
{
	class CommandTextsliderStateful : public CommandTextslider
	{
	protected:
		long long default_value;

	public:
		explicit CommandTextsliderStateful(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandTextsliderData>&& options, const commandflags_t flags = CMDFLAGS_SLIDER_TEXT, const CommandPerm perms = COMMANDPERM_USERONLY);

		void onFocus(ThreadContext thread_context, Direction momentum) override;

		std::string getState() const final;
		[[nodiscard]] std::string getWebState() const;
		void setState(Click& click, const std::string& state) final;
		void applyDefaultState() final;
		void processValueChange(const Click& click) const final;
	};
}
