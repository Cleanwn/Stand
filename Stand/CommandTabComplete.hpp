#pragma once

#include "CommandToggleRegular.hpp"

#include "Renderer.hpp"

namespace Stand
{
	class CommandTabComplete : public CommandToggleRegular
	{
	public:
		inline static std::vector<Hotkey> default_hotkeys{ VK_TAB };

		explicit CommandTabComplete(CommandList* const parent)
			: CommandToggleRegular(parent, LOC("TBCMPLT"), { CMDNAME("tabcomplete") }, LOC("TBCMPLT_H"), false, default_hotkeys)
		{
		}

		[[nodiscard]] const std::vector<Hotkey>& getDefaultHotkeys() const noexcept final
		{
			return default_hotkeys;
		}

		void onEnableInner(Click& click) final
		{
			g_renderer.tab_complete_keys = hotkeys;
		}

		void onDisableInner(Click& click) final
		{
			g_renderer.tab_complete_keys.clear();
		}

		void onHotkeysChanged(ClickType type) const final
		{
			if (m_on)
			{
				g_renderer.tab_complete_keys = hotkeys;
			}
		}
	};
}
