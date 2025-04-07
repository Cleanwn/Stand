#pragma once

#include "CommandToggleNoCorrelation.hpp"

#include "ToggleCorrelation.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandToggle : public CommandToggleNoCorrelation
	{
	public:
		ToggleCorrelation correlation{};
		ToggleCorrelation default_correlation{};

		explicit CommandToggle(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const bool default_on = false, const commandflags_t flags = CMDFLAGS_TOGGLE, const CommandPerm perm = COMMANDPERM_USERONLY, const std::vector<Hotkey>& default_hotkeys = {});
		explicit CommandToggle(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, ToggleCorrelation::Type correlation);
		
		void preDelete() override;

		void onClick(Click& click) override;
	protected:
		void onClickRemoveCorrelation(Click& click);

	public:
		std::string getState() const override;
		std::string getDefaultState() const final;
		void setState(Click& click, const std::string& _state) override;
		void applyDefaultState() override;

		void setCorrelation(Click& click, const ToggleCorrelation& correlation);
		void setCorrelation(Click& click, ToggleCorrelation::Type correlation, bool correlation_invert);
		void removeCorrelation(Click& click);
		void processCorrelationUpdate(Click& click);
		void setStateBasedOnCorrelation(Click& click);
		void setStateBasedOnCorrelation(Click& click, bool value);
		void toggleCorrelation(Click& click, bool on, ToggleCorrelation::Type correlation, bool correlation_invert);
	};
#pragma pack(pop)
}
