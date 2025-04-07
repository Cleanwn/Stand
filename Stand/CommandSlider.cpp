#include "CommandSlider.hpp"

#include <stdexcept>

#include <fmt/xchar.h>

#include "CommandSliderFloat.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "get_next_arg.hpp"
#include "Gui.hpp"
#include "input.hpp"
#include "RelayCon.hpp"
#include "str2int.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandSlider::CommandSlider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const unsigned int step_size, const commandflags_t flags, const CommandPerm perm, bool is_click_to_apply, const std::vector<Hotkey>& default_hotkeys, CommandType type)
		: CommandPhysical(type, parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, perm, default_hotkeys), min_value(min_value), max_value(max_value), value(default_value), default_value(default_value), step_size(step_size), is_click_to_apply(is_click_to_apply)
	{
	}

	bool CommandSlider::isNotApplicable() const noexcept
	{
		return min_value == max_value;
	}

	bool CommandSlider::isSelectable() const noexcept
	{
		return !isNotApplicable() || selectable_na;
	}

	void CommandSlider::onFocus(ThreadContext thread_context, Direction momentum)
	{
		if (!isSelectable())
		{
			if (g_gui.getCurrentUiList()->canUpdateCursor())
			{
				if (momentum == UP)
				{
					g_gui.inputUp(thread_context);
				}
				else if (momentum == DOWN)
				{
					g_gui.inputDown(thread_context);
				}
			}
		}
	}

	float CommandSlider::getFloatValue() const
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(this->type == COMMAND_SLIDER_FLOAT);
#endif
		return as<CommandSliderFloat>()->getFloatValue();
	}

	std::wstring CommandSlider::formatNumber(int num, bool allow_replacements) const
	{
		return fmt::to_wstring(num);
	}

	bool CommandSlider::currentNumberHasReplacement() const
	{
		return formatNumber(value, false) != formatNumber(value, true);
	}

	std::wstring CommandSlider::getCommandSyntax() const
	{
		if (command_names.empty())
		{
			return {};
		}
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [").append(getRangeString()).append(L"]");
		return syntax;
	}

	std::wstring CommandSlider::getRangeString() const
	{
		std::wstring range = formatNumber(min_value, false);
		range.push_back(L' ');
		range.append(LANG_GET_W("ARGRNGE"));
		range.push_back(L' ');
		range.append(formatNumber(max_value, false));
		return range;
	}

	void CommandSlider::onClick(Click& click)
	{
		if (click.canOpenCommandBox()
			&& !click.isCommandBoxVariety()
			&& !click.isWeb()
			&& !command_names.empty()
			)
		{
			openCommandBoxWithPrefill(click);
		}
	}

	void CommandSlider::onCommand(Click& click, std::wstring& args)
	{
		auto arg = get_next_arg(args);
		if (arg.empty())
		{
			if (click.canOpenCommandBox())
			{
				openCommandBoxWithPrefill(click);
			}
			return;
		}
		StringUtils::replace_all(arg, L".", L"");
		StringUtils::replace_all(arg, L",", L"");
		StringUtils::to_lower(arg);
		const int prev_value = value;
		if (onCommandImplKeyword(arg)
			|| onCommandImplValue(click, std::move(arg))
			)
		{
			onCommandImplDispatch(click, prev_value);
		}
	}

	bool CommandSlider::onCommandImplKeyword(const std::wstring& arg)
	{
		if (arg == L"min")
		{
			value = min_value;
			return true;
		}
		else if (arg == L"max")
		{
			value = max_value;
			return true;
		}
		else if (arg == L"default"
			|| arg == L"off"
			|| arg == L"disable"
			|| arg == L"disabled"
			)
		{
			value = default_value;
			return true;
		}
		return false;
	}

	bool CommandSlider::onCommandImplValue(Click& click, std::wstring&& arg)
	{
		auto res = str2int<int>(arg);
		if (!res.has_value())
		{
			click.setResponse(LOC("INVARG"));
			std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
			prefill.push_back(L' ');
			prefill.append(arg);
			click.showCommandBoxIfPossible(std::move(prefill));
			return false;
		}
		value = res.value();
		if (value > max_value)
		{
			value = max_value;
		}
		else if (value < min_value)
		{
			value = min_value;
		}
		return true;
	}

	void CommandSlider::onCommandImplDispatch(Click& click, int prev_value)
	{
		updateState(click);
		Click click_(click);
		onClick(click_);
		if (click_.hasResponse())
		{
			click.setNoResponse();
		}
		onChange(click, prev_value);
	}

	void CommandSlider::openCommandBoxWithPrefill(Click& click)
	{
		std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
		prefill.push_back(L' ');
		if (prefill_value
			&& !currentNumberHasReplacement()
			)
		{
			prefill.append(formatNumber(value));
		}
		click.showCommandBoxIfPossible(std::move(prefill));
	}

	bool CommandSlider::onLeft(Click& click, bool holding)
	{
		bool ret;
		int value = this->value;
		const bool was_above_default_value = holding && value > default_value;
		value -= step_size;
		if (was_above_default_value && value <= default_value)
		{
			value = default_value;
			ret = false;
		}
		else if (value < min_value)
		{
			if (holding || value + step_size != min_value)
			{
				value = min_value;
				ret = false;
			}
			else
			{
				value = max_value;
				ret = true;
			}
		}
		else
		{
			ret = true;
		}
		if (this->value != value)
		{
			updateValue(click, value);
		}
		return ret;
	}

	bool CommandSlider::onRight(Click& click, bool holding)
	{
		bool ret;
		int value = this->value;
		const bool was_below_default_value = holding && value < default_value;
		value += step_size;
		if (was_below_default_value && value >= default_value)
		{
			value = default_value;
			ret = false;
		}
		else if (value > max_value
			|| value < this->value // integer overflowed?
			)
		{
			if (holding || value - step_size != max_value)
			{
				value = max_value;
				ret = false;
			}
			else
			{
				value = min_value;
				ret = true;
			}
		}
		else
		{
			ret = true;
		}
		if (this->value != value)
		{
			updateValue(click, value);
		}
		return ret;
	}

	void CommandSlider::updateValue(Click& click, int value)
	{
		if (click.canHaveGenericResponse())
		{
			click.setGenericResponse(LIT(LANG_FMT("CMDSTATE_L", getActivationName().getLocalisedUtf8(), value)));
		}
		ensureScriptThread(click, [=](Click& click) mutable
		{
			const int prev_value = this->value;
			this->value = value;
			updateState(click);
			onChange(click, prev_value);
		});
	}

	void CommandSlider::setValue(Click& click, int value)
	{
		if (value < min_value)
		{
			value = min_value;
		}
		else if (value > max_value)
		{
			value = max_value;
		}
		if (this->value != value)
		{
			const int prev_value = this->value;
			this->value = value;
			if (click.isWeb())
			{
				Click click_(click);
				onClick(click_);
			}
			updateState(click);
			onChange(click, prev_value);
		}
	}

	std::string CommandSlider::getState() const
	{
		return fmt::to_string(value);
	}

	std::string CommandSlider::getDefaultState() const
	{
		return fmt::to_string(default_value);
	}

	void CommandSlider::setState(Click& click, const std::string& state)
	{
		int updated_value = default_value;
		if (!state.empty())
		{
			std::string state_{ state };
			StringUtils::replace_all(state_, ".", "");
			StringUtils::replace_all(state_, ",", "");
			StringUtils::replace_all(state_, " ", "");
			try
			{
				updated_value = std::stoi(state_);
			}
			catch (const std::exception&)
			{
			}
		}
		setValue(click, updated_value);
	}

	void CommandSlider::applyDefaultState()
	{
		if (value != default_value)
		{
			const int prev_value = value;
			value = default_value;
			Click click(CLICK_BULK, TC_APPLYDEFAULTSTATE);
			updateState(click);
			onChange(click, prev_value);
		}
	}

	void CommandSlider::onChange(Click& click, int prev_value)
	{
	}

	void CommandSlider::onChangeImplUnavailable(Click& click, int prev_value)
	{
		click.setUnavailable();
		setValueIndicator(click, prev_value);
	}

	void CommandSlider::setValueIndicator(const Click& click, int value)
	{
		this->value = value;
		updateState(click);
	}

	void CommandSlider::updateState(const Click& click)
	{
		processVisualUpdate(false);
		if (click.type != CLICK_BULK && supportsStateOperations())
		{
			g_gui.on_command_state_change();
		}
		if (!click.isWeb() && isActiveOnWeb())
		{
			Exceptional::createManagedExceptionalThread(__FUNCTION__, [this]
			{
				EXCEPTIONAL_LOCK(g_relay.send_mtx)
				g_relay.sendLine(std::move(std::string("e ").append(menu_name.getWebString())));
				g_relay.sendLine(std::move(std::string("s ").append(fmt::to_string(value))));
				EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
			});
		}
	}

	void CommandSlider::setMinValue(int min_value)
	{
		if (min_value > max_value)
		{
			min_value = max_value;
		}
		if (this->min_value != min_value)
		{
			this->min_value = min_value;
			processVisualUpdate();
			//update_web_min_value();
		}
	}

	void CommandSlider::setMaxValue(int max_value)
	{
		if (max_value < min_value)
		{
			max_value = min_value;
		}
		if (this->max_value != max_value)
		{
			this->max_value = max_value;
			processVisualUpdate();
			updateWebMaxValue();
		}
	}

	void CommandSlider::setStepSize(int step_size)
	{
		//if (this->skip != step_size)
		{
			this->step_size = step_size;
			//update_web_step_size();
		}
	}

	void CommandSlider::updateWebMaxValue() const
	{
		if (isActiveOnWeb())
		{
			Exceptional::createManagedExceptionalThread(__FUNCTION__, [this]
			{
				EXCEPTIONAL_LOCK(g_relay.send_mtx)
				g_relay.sendLine(std::move(std::string("e ").append(menu_name.getWebString())));
				g_relay.sendLine(std::move(std::string("m ").append(fmt::to_string(max_value))));
				EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
			});
		}
	}
}
