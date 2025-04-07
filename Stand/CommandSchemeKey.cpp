#include "CommandSchemeKey.hpp"

#include "Gui.hpp"

namespace Stand
{
	void CommandSchemeKey::onClick(Click& click)
	{
		openHotkeysList(click.thread_context);
	}

	const std::vector<Hotkey>& CommandSchemeKey::getDefaultHotkeys() const noexcept
	{
		return default_hotkeys;
	}

	void CommandSchemeKey::onHotkeysChanged(ClickType type) const
	{
		*scheme_key = hotkeys;
	}

	void CommandSchemeKey::setHotkeysFromScheme()
	{
		hotkeys = *scheme_key;
		g_gui.hotkeys.data[getPathConfig()] = hotkeys;
	}
}
