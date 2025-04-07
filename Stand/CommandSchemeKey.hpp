#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandSchemeKey : public CommandAction
	{
	public:
		std::vector<Hotkey>* const scheme_key;
		std::vector<Hotkey> default_hotkeys;

		explicit CommandSchemeKey(CommandList* const parent, Label&& menu_name, std::vector<Hotkey>* const scheme_key, std::vector<Hotkey>&& default_hotkeys)
			: CommandAction(parent, std::move(menu_name), {}, NOLABEL, 0, COMMANDPERM_USERONLY, default_hotkeys), scheme_key(scheme_key), default_hotkeys(std::move(default_hotkeys))
		{
		}

		void onClick(Click& click) final;

		[[nodiscard]] const std::vector<Hotkey>& getDefaultHotkeys() const noexcept final;
		void onHotkeysChanged(ClickType type) const final;

		void setHotkeysFromScheme();
	};
}
