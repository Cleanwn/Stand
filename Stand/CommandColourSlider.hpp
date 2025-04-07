#pragma once

#include "CommandSlider.hpp"

#include "CommandColour.hpp"

namespace Stand
{
	class CommandColourSlider : public CommandSlider
	{
	public:
		enum ValueType : uint8_t
		{
			RGB,
			HSV,
			OPACITY,
		};

	private:
		const ValueType value_type;

	public:
		explicit CommandColourSlider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, const ValueType value_type, const int min_value, const int max_value, const int default_value)
			: CommandSlider(parent, std::move(menu_name), std::move(command_names), NOLABEL, min_value, max_value, default_value, 1, (CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_FEATURELIST_SKIP), value_type(value_type)
		{
		}

		void onCommand(Click& click, std::wstring& args) final;

		Label getActivationName() const final;

		void onChange(Click& click, int prev_value) final;
	};
}
