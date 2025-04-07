#include "CommandInput.hpp"

#include "Auth.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"
#include "UnicodePrivateUse.hpp"

namespace Stand
{
	CommandInput::CommandInput(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::string&& default_value, std::string&& value, const commandflags_t flags, const CommandPerm perm)
		: CommandPhysical(COMMAND_INPUT, parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, perm), default_value(std::move(default_value)), value(std::move(value))
	{
		mustHaveCommandName();
	}

	CommandInput::CommandInput(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::string&& default_value, const commandflags_t flags, const CommandPerm perm)
		: CommandInput(parent, std::move(menu_name), std::move(command_names), std::move(help_text), std::string(default_value), std::move(default_value), flags, perm)
	{
	}

	std::wstring CommandInput::getCommandSyntax() const
	{
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
		syntax.append(LANG_GET_W("ARGVAL"));
		return syntax;
	}

	void CommandInput::onClick(Click& click)
	{
		std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
		prefill.push_back(L' ');
		if (prefill_value || click.isWeb())
		{
			prefill.append(StringUtils::utf8_to_utf16(value));
		}
		click.showCommandBoxIfPossible(std::move(prefill));
	}

	void CommandInput::onCommand(Click& click, std::wstring& args)
	{
		onCommandApply(click, args);
	}

	void CommandInput::onCommandApply(Click& click, std::wstring& args)
	{
		setValueGta(StringUtils::utf16_to_utf8(args), click.type);
		args.clear();
	}

	std::string CommandInput::getState() const
	{
		return value;
	}

	std::string CommandInput::getDefaultState() const
	{
		return default_value;
	}

	void CommandInput::setState(Click& click, const std::string& state)
	{
		if (value != state)
		{
			auto args = StringUtils::utf8_to_utf16(state);
			onCommand(click, args);
		}
	}

	void CommandInput::applyDefaultState()
	{
		Click click(CLICK_BULK, TC_APPLYDEFAULTSTATE);
		setState(click, default_value);
	}

	void CommandInput::setValue(std::string&& value, ClickType type)
	{
		this->value = std::move(value);
		update_state(type);
	}

	void CommandInput::setValueGta(const std::string& value, ClickType type)
	{
		auto value_utf16 = StringUtils::utf8_to_utf16(value);
		UnicodePrivateUse::fromGta(value_utf16);
		setValue(StringUtils::utf16_to_utf8(value_utf16), type);
	}

	void CommandInput::update_state(ClickType type) const
	{
		processVisualUpdate();
		if (type != CLICK_BULK && supportsStateOperations())
		{
			g_gui.on_command_state_change();
		}
	}

	void CommandInput::update_state(const Click& click) const
	{
		update_state(click.type);
	}
}
