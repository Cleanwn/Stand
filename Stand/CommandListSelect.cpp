#include "CommandListSelect.hpp"

#include <optional>

#include <soup/os.hpp>

#include "CommandListActionItem.hpp"
#include "get_next_arg.hpp"
#include "Gui.hpp"
#include "regular_event.hpp"
#include "RelayCon.hpp"

namespace Stand
{
	CommandListSelect::CommandListSelect(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandListActionItemData>&& options, long long default_value, const commandflags_t flags, const commandflags_t item_flags)
		: CommandListAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text), std::move(options), flags, item_flags, COMMAND_LIST_SELECT), default_value(default_value), value(default_value)
	{
	}

	std::wstring CommandListSelect::getCommandSyntax() const
	{
		if (command_names.empty())
		{
			return {};
		}
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0)));
		syntax.push_back(L' ');
		syntax.append(LANG_GET_W("ARGVALOPT"));
		return syntax;
	}

	void CommandListSelect::onClick(Click& click)
	{
		if (!click.isWeb())
		{
			CommandListAction::onClick(click);

			// Focus the option or the category the option belongs to
			if (auto cmd = getValueCommand(value))
			{
				if (cmd->parent->type == COMMAND_LIST)
				{
					cmd->parent->focusInParent(click.thread_context);
				}
				else
				{
					cmd->focusInParent(click.thread_context);
				}
			}
		}
	}

	bool CommandListSelect::onLeft(Click& click, bool holding)
	{
		if (!job_queued)
		{
			long long prev_value = value;
			std::optional<long long> prev = std::nullopt;
			for (const auto& option : options)
			{
				if (option.value == value)
				{
					if (prev.has_value())
					{
						value = prev.value();
						onBack(click.thread_context);
						setGenericResponse(click);
						updateState(click);
						onChange(click, prev_value);
					}
					else
					{
						if (holding)
						{
							return false;
						}
						value = options[options.size() - 1].value;
						onBack(click.thread_context);
						setGenericResponse(click);
						updateState(click);
						onChange(click, prev_value);
					}
					break;
				}
				prev = option.value;
			}
		}
		return true;
	}

	bool CommandListSelect::onRight(Click& click, bool holding)
	{
		if (!job_queued)
		{
			long long prev_value = value;
			bool next = false;
			for (const auto& option : options)
			{
				if (next)
				{
					value = option.value;
					onBack(click.thread_context);
					setGenericResponse(click);
					updateState(click);
					onChange(click, prev_value);
					return true;
				}
				if (option.value == value)
				{
					next = true;
				}
			}
			if (next)
			{
				if (holding)
				{
					return false;
				}
				value = options[0].value;
				onBack(click.thread_context);
				setGenericResponse(click);
				updateState(click);
				onChange(click, prev_value);
			}
		}
		return true;
	}

	CommandListActionItem* CommandListSelect::getCurrentValueCommand() const
	{
		return getValueCommand(value);
	}

	Label CommandListSelect::getCurrentValueMenuName() const
	{
		return getValueMenuName(value);
	}

	Label CommandListSelect::getDefaultValueMenuName() const
	{
		return getValueMenuName(default_value);
	}

	Label CommandListSelect::getCurrentValueHelpText() const
	{
		if (auto data = getValueData(value))
		{
			return data->help_text;
		}
		return NOLABEL;
	}

	void CommandListSelect::setValue(Click& click, long long value)
	{
		if (this->value != value)
		{
			for (const auto& option : options)
			{
				if (value == option.value)
				{
					long long prev_value = this->value;
					this->value = option.value;
					onChange(click, prev_value);
					updateState(click);
					return;
				}
			}
		}
	}

	void CommandListSelect::setValueIndication(long long value)
	{
		if (this->value != value)
		{
			this->value = value;
			processVisualUpdate(true);
			updateWebState();
		}
	}

	std::string CommandListSelect::getState() const
	{
		return getCurrentValueMenuName().getEnglishUtf8();
	}

	std::string CommandListSelect::getDefaultState() const
	{
		return getDefaultValueMenuName().getEnglishUtf8();
	}

	void CommandListSelect::setState(Click& click, const std::string& state)
	{
		if (state.empty())
		{
			applyDefaultState();
		}
		else
		{
			for (const auto& option : options)
			{
				if (state == option.menu_name.getEnglishUtf8())
				{
					if (this->value != option.value)
					{
						long long prev_value = value;
						this->value = option.value;
						onChange(click, prev_value);
						updateState(click);
					}
					return;
				}
			}
			if (state == "On")
			{
				setState(click, "Enabled");
			}
			else if (state == "Off")
			{
				setState(click, "Disabled");
			}
		}
	}

	void CommandListSelect::applyDefaultState()
	{
		if (value != default_value)
		{
			Click click(CLICK_BULK, TC_APPLYDEFAULTSTATE);
			setValue(click, default_value);
		}
	}

	void CommandListSelect::onCommand(Click& click, std::wstring& args)
	{
		auto og_args = args;
		auto arg = get_next_arg(args);
		if (arg == L"default")
		{
			auto data = getValueData(default_value);
			if (data && !data->command_suffixes.empty())
			{
				auto new_args = cmdNameToUtf16(data->command_suffixes.at(0));
				return CommandListAction::onCommand(click, new_args);
			}
		}
		args = std::move(og_args);
		return CommandListAction::onCommand(click, args);
	}

	void CommandListSelect::onItemClick(Click& click, CommandListActionItem* item)
	{
		long long prev_value = value;
		const bool changed = (value != item->value);
		if (changed)
		{
			value = item->value;
		}
		CommandListAction::onItemClick(click, item);
		if (changed)
		{
			setGenericResponse(click);
			onChange(click, prev_value);
			updateState(click);
		}
		else if (click.canHaveGenericErrorResponse())
		{
			click.setResponse(LIT(LANG_FMT("CMDSTATE_L_E", getActivationName().getLocalisedUtf8(), item->menu_name.getLocalisedUtf8())));
		}
	}

	void CommandListSelect::onChange(Click& click, long long prev_value)
	{
	}

	void CommandListSelect::onChangeImplWarning(Click& click, long long prev_value, std::wstring&& warning, warningflags_t flags)
	{
		const auto intended_value = this->value;
		auto click_ = click.deriveAuto();
		setValue(click_, prev_value);
		showWarning(click, std::move(warning), flags, [this, intended_value](ThreadContext thread_context)
		{
			Click click(CLICK_AUTO, thread_context);
			setValue(click, intended_value);
		});
	}

	void CommandListSelect::onChangeImplUnavailable(Click& click, long long prev_value)
	{
		click.setUnavailable();
		setValueIndication(prev_value);
	}

	void CommandListSelect::onChangeProcessScriptTickEventHandler(Click& click, long long prev_value, long long for_value, void handler())
	{
		if (this->value == for_value)
		{
			reScriptTickEvent::registerHandler(handler);
		}
		else if (prev_value == for_value)
		{
			reScriptTickEvent::unregisterHandler(handler);
		}
	}

	void CommandListSelect::setGenericResponse(Click& click) const
	{
		if (click.canHaveGenericResponse())
		{
			click.setGenericResponse(LIT(LANG_FMT("CMDSTATE_L", getActivationName().getLocalisedUtf8(), getCurrentValueMenuName().getLocalisedUtf8())));
		}
	}

	void CommandListSelect::updateState(const Click& click) const
	{
		processVisualUpdate(false);
		if (click.type != CLICK_BULK && supportsStateOperations())
		{
			g_gui.on_command_state_change();
		}
		if (!click.isWeb())
		{
			updateWebState();
		}
	}

	std::string CommandListSelect::getWebState() const
	{
		return getCurrentValueMenuName().getWebString();
	}

	void CommandListSelect::sendDataToRelay() const
	{
		g_relay.sendLine(std::move(std::string("d ").append(menu_name.getWebString())));
		uint8_t i = 0;
		for (const auto& option : options)
		{
			g_relay.sendLine(std::move(std::string("> ").append(option.menu_name.getWebString())));
			if (++i == 50)
			{
				i = 0;
				soup::os::sleep(1);
			}
		}
		g_relay.sendLine(std::move(std::string("s ").append(getWebState())));
	}

	void CommandListSelect::updateWebState() const
	{
		if (isActiveOnWeb())
		{
			Exceptional::createManagedExceptionalThread(__FUNCTION__, [this]
			{
				EXCEPTIONAL_LOCK(g_relay.send_mtx)
				g_relay.sendLine(std::move(std::string("e ").append(menu_name.getWebString())));
				g_relay.sendLine(std::move(std::string("s ").append(getWebState())));
				EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
			});
		}
	}
}
