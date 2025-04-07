#include "CommandCtxHotkey.hpp"

#include "CommandCtxHotkeyHoldMode.hpp"
#include "CommandCtxHotkeyRemove.hpp"
#include "CommandLambdaAction.hpp"
#include "Gui.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandCtxHotkey::CommandCtxHotkey(CommandList* parent, Hotkey* hotkey, CommandPhysical* target)
		: CommandList(parent, LIT(hotkey->toString())), hotkey(hotkey)
	{
		if (target->canHotkeyBeRemoved(*hotkey))
		{
			this->createChild<CommandCtxHotkeyRemove>(target);
		}
		else
		{
			this->createChild<CommandLambdaAction>(LOC("HOTKEY_C2"), {}, NOLABEL, [this](Click& click)
			{
				this->goBackIfActive(click.thread_context);
				g_gui.changeHotkeyOnFocusedCommand();
			});
		}
		if (target->isToggle())
		{
			this->createChild<CommandCtxHotkeyHoldMode>();
		}
	}

	void CommandCtxHotkey::save(CommandPhysical* target)
	{
		g_gui.hotkeys.save();
		Util::toast(LANG_FMT("HOTKEY_T", target->getActivationName().getLocalisedUtf8()), TOAST_ABOVE_MAP);
	}
}
