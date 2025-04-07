#include "CommandCtxHotkeys.hpp"

#include "CommandCtxHotkey.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "ContextMenu.hpp"
#include "Gui.hpp"

namespace Stand
{
	CommandCtxHotkeys::CommandCtxHotkeys(CommandList* const parent)
		: CommandList(parent, LOC("HOTKEYS"))
	{
		populate();
	}

	void CommandCtxHotkeys::onBack(ThreadContext thread_context)
	{
		if (close_context_menu_on_back)
		{
			ensureYieldableScriptThread(thread_context, []
			{
				ContextMenu::close(TC_SCRIPT_YIELDABLE);
			});
		}
	}

	void CommandCtxHotkeys::populate()
	{
		CommandPhysical* const target = ContextMenu::getTargetPhysical();

		std::vector<CommandName> command_names{};
		if (!target->command_names.empty())
		{
			CommandName command_name{ CMDNAME("hotkeyadd ") };
			command_name.append(target->command_names.at(0)).push_back(' ');
			command_name.append(utf8ToCmdName(LANG_GET("ARGHOTKEY")));
			command_names.emplace_back(std::move(command_name));
		}
		this->createChild<CommandLambdaAction>(LOC("HOTKEY_A"), std::move(command_names), NOLABEL, [this](Click&)
		{
			g_gui.addHotkeyToFocusedCommand();
		});
		if (!target->hotkeys.empty())
		{
			this->createChild<CommandDivider>(LOC("HOTKEYS"));
			for (auto& hotkey : target->hotkeys)
			{
				this->createChild<CommandCtxHotkey>(&hotkey, target);
			}
		}
	}

	void CommandCtxHotkeys::update()
	{
		resetChildren();
		populate();
		processChildrenUpdate();
	}
}
