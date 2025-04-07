#include "CommandInputScheme.hpp"

#include "CommandListAction.hpp"
#include "CommandListActionItem.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "input.hpp"
#include "lang.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandInputSchemePresets : public CommandListAction
	{
	private:
		using preset_t = void(*)(Click& click, CommandInputScheme* cmd);

		static void preset_flexible(Click& click, CommandInputScheme* cmd)
		{
			Input::scheme = InputScheme();
			cmd->onSchemeUpdate();
		}

		static void preset_no_numpad(Click& click, CommandInputScheme* cmd)
		{
			Input::scheme = InputSchemeNoNumpad();
			cmd->onSchemeUpdate();
		}

		static void preset_numpad(Click& click, CommandInputScheme* cmd)
		{
			if (Input::user_has_numpad)
			{
				Input::scheme = InputSchemeNumpad();
				cmd->onSchemeUpdate();
			}
			else
			{
				click.setResponse(LOC("KSCHM_1_E"));
			}
		}

	public:
		explicit CommandInputSchemePresets(CommandList* const parent)
			: CommandListAction(parent, LOC("PRSTS"), CMDNAMES("inputpreset", "inputscheme"), NOLABEL, {
				{(long long)&preset_flexible, LOC("KSCHM_A"), CMDNAMES("flexible")},
				{(long long)&preset_no_numpad, LOC("KSCHM_0"), CMDNAMES("tkl")},
				{(long long)&preset_numpad, LOC("KSCHM_1"), CMDNAMES("full")},
			})
		{
		}

		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			((preset_t)item->value)(click, (CommandInputScheme*)parent);
			return CommandListAction::onItemClick(click, item);
		}
	};

	CommandInputScheme::CommandInputScheme(CommandList* const parent)
		: CommandList(parent, LOC("KSCHM"), { CMDNAME("inputscheme"), CMDNAME("keyboardinputscheme") })
	{
		createChild<CommandInputSchemePresets>();

		key_menu_open_close = createChild<CommandSchemeKey>(LOC("B_OC"), &Input::scheme.key_menu_open_close, std::vector<Hotkey>{ VK_F4, VK_ADD });
		key_menu_root_up = createChild<CommandSchemeKey>(LOC("B_RU"), &Input::scheme.key_menu_root_up, std::vector<Hotkey>{ VK_RSHIFT, VK_NUMPAD7 });
		key_menu_root_down = createChild<CommandSchemeKey>(LOC("B_RD"), &Input::scheme.key_menu_root_down, std::vector<Hotkey>{ VK_RCONTROL, VK_NUMPAD1 });
		key_menu_up = createChild<CommandSchemeKey>(LOC("B_MU"), &Input::scheme.key_menu_up, std::vector<Hotkey>{ VK_UP, VK_NUMPAD8 });
		key_menu_down = createChild<CommandSchemeKey>(LOC("B_MD"), &Input::scheme.key_menu_down, std::vector<Hotkey>{ VK_DOWN, VK_NUMPAD2 });
		key_menu_left = createChild<CommandSchemeKey>(LOC("B_ML"), &Input::scheme.key_menu_left, std::vector<Hotkey>{ VK_LEFT, VK_NUMPAD4 });
		key_menu_right = createChild<CommandSchemeKey>(LOC("B_MR"), &Input::scheme.key_menu_right, std::vector<Hotkey>{ VK_RIGHT, VK_NUMPAD6 });
		key_menu_click = createChild<CommandSchemeKey>(LOC("B_MC"), &Input::scheme.key_menu_click, std::vector<Hotkey>{ VK_RETURN, VK_NUMPAD5 });
		key_menu_back = createChild<CommandSchemeKey>(LOC("B_MB"), &Input::scheme.key_menu_back, std::vector<Hotkey>{ VK_BACK, VK_NUMPAD0 });
		key_menu_context = createChild<CommandSchemeKey>(LOC("B_CTX"), &Input::scheme.key_menu_context, std::vector<Hotkey>{ 'O', VK_NUMPAD3 });
		key_command_box = createChild<CommandSchemeKey>(LOC("B_CB"), &Input::scheme.key_command_box, std::vector<Hotkey>{ 'U' });
	}

	void CommandInputScheme::setHotkeysFromScheme()
	{
		key_menu_open_close->setHotkeysFromScheme();
		key_menu_root_up->setHotkeysFromScheme();
		key_menu_root_down->setHotkeysFromScheme();
		key_menu_up->setHotkeysFromScheme();
		key_menu_down->setHotkeysFromScheme();
		key_menu_left->setHotkeysFromScheme();
		key_menu_right->setHotkeysFromScheme();
		key_menu_click->setHotkeysFromScheme();
		key_menu_back->setHotkeysFromScheme();
		key_menu_context->setHotkeysFromScheme();
		key_command_box->setHotkeysFromScheme();
	}

	void CommandInputScheme::onSchemeUpdate()
	{
		setHotkeysFromScheme();
		g_gui.hotkeys.save();
	}
}
