#pragma once

#include "CommandToggleRegular.hpp"

#include "Renderer.hpp"

namespace Stand
{
	class CommandPaste : public CommandToggleRegular
	{
	public:
		inline static std::vector<Hotkey> default_hotkeys{ Hotkey('V', true, false, false) };

		explicit CommandPaste(CommandList* const parent)
			: CommandToggleRegular(parent, LOC("PSTE"), { CMDNAME("chatpaste") }, LOC("PSTE_H"), false, default_hotkeys)
		{
		}

		[[nodiscard]] const std::vector<Hotkey>& getDefaultHotkeys() const noexcept final
		{
			return default_hotkeys;
		}

		void onEnableInner(Click& click) final
		{
			g_renderer.paste_keys = hotkeys;
		}

		void onDisableInner(Click& click) final
		{
			g_renderer.paste_keys.clear();
		}

		void onHotkeysChanged(ClickType type) const final
		{
			if (m_on)
			{
				g_renderer.paste_keys = hotkeys;
			}
		}
	};
}
