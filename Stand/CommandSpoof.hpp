#pragma once

#include "CommandList.hpp"

#include "CommandSpoofSlider.hpp"
#include "CommandSpoofToggle.hpp"

namespace Stand
{
	template <class CSlider = CommandSlider>
	class CommandSpoof : public CommandList
	{
	protected:
		CSlider* slider;

	public:
		explicit CommandSpoof(CommandList* parent, Label&& toggle_name, Label&& value_name, std::vector<CommandName>&& command_suffixes, Label&& help_text = NOLABEL)
			: CommandList(parent, std::move(toggle_name), {}, std::move(help_text))
		{
			this->createChild<CommandSpoofToggle>(Label(this->menu_name), CommandIssuable::combineCommandNames(CMDNAME("spoof"), command_suffixes), Label(this->help_text));
			slider = this->createChild<CommandSpoofSlider<CSlider>>(std::move(value_name), CommandIssuable::combineCommandNames(CMDNAME("spoofed"), command_suffixes), NOLABEL, 0x80000000, 0x7FFFFFFF, 1, 1);
		}

		virtual void onToggle(bool on) const
		{
		}

		virtual void onValueChange()
		{
		}

		virtual void onTick() const = 0;
	};
}
