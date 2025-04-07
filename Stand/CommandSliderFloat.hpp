#pragma once

#include "CommandSlider.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandSliderFloat : public CommandSlider
	{
	public:
		uint8_t precision = 2;

		explicit CommandSliderFloat(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const unsigned int step_size = 1, const commandflags_t flags = CMDFLAGS_SLIDER, const CommandPerm perm = COMMANDPERM_USERONLY, bool is_click_to_apply = false, const std::vector<Hotkey>& default_hotkeys = {});

		[[nodiscard]] int getPrecisionScalar() const;
		[[nodiscard]] float getFloatValue() const;
		[[nodiscard]] std::wstring formatNumber(int num, bool allow_replacements) const override;

		void onCommand(Click& click, std::wstring& args) override;

		[[nodiscard]] std::string getState() const final;
		[[nodiscard]] std::string getDefaultState() const final;

		void setValue(float value, Click& click);
	};
#pragma pack(pop)
}
