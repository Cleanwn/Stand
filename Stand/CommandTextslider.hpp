#pragma once

#include "CommandPhysical.hpp"

#include "CommandTextsliderData.hpp"

namespace Stand
{
	class CommandTextslider : public CommandPhysical
	{
	public:
		std::vector<CommandTextsliderData> options;

		long long value;

		explicit CommandTextslider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandTextsliderData>&& options, const commandflags_t flags = CMDFLAGS_ACTION_SLIDER, const CommandPerm perms = COMMANDPERM_USERONLY, const CommandType type = COMMAND_TEXTSLIDER);

		[[nodiscard]] bool hasOptions() const noexcept;
		[[nodiscard]] bool hasOption(long long value) const noexcept;
		void setOptions(std::vector<CommandTextsliderData>&& options);

		void resetValue();

		[[nodiscard]] Label getCurrentValueLabel() const;

		void onFocus(ThreadContext thread_context, Direction momentum) override;

		bool onLeft(Click& click, bool holding) final;
		bool onRight(Click& click, bool holding) final;
		virtual void processValueChange(const Click& click) const;
	};
}
