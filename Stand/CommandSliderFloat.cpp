#include "CommandSliderFloat.hpp"

#include <fmt/xchar.h>

#include <soup/math.hpp>

#include "get_next_arg.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandSliderFloat::CommandSliderFloat(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const unsigned int step_size, const commandflags_t flags, const CommandPerm perm, bool is_click_to_apply, const std::vector<Hotkey>& default_hotkeys)
		: CommandSlider(parent, std::move(menu_name), std::move(command_names), std::move(help_text), min_value, max_value, default_value, step_size, flags, perm, is_click_to_apply, default_hotkeys, COMMAND_SLIDER_FLOAT)
	{
	}

	int CommandSliderFloat::getPrecisionScalar() const
	{
		return soup::pow<int>(10, precision);
	}

	float CommandSliderFloat::getFloatValue() const
	{
		return (float)value / (float)getPrecisionScalar();
	}

	std::wstring CommandSliderFloat::formatNumber(int num, bool allow_replacements) const
	{
		const auto scalar = getPrecisionScalar();

		// {}{}.{:0>X} where X is precision
		std::wstring fstr = L"{}{}.{:0>";
		fstr.append(fmt::to_wstring(precision));
		fstr.push_back(L'}');
		return fmt::format(fmt::runtime(std::move(fstr)), num < 0 ? L"-" : L"", abs(num / scalar), abs(num % scalar));
	}

	void CommandSliderFloat::onCommand(Click& click, std::wstring& args)
	{
		auto arg = get_next_arg(args);
		if (arg.empty())
		{
			return onClick(click);
		}
		const int prev_value = value;
		if (!onCommandImplKeyword(arg))
		{
			if (arg.find('.') == std::string::npos
				&& arg.find(',') == std::string::npos) // 10 -> 10.00
			{
				arg.append(precision, '0');
			}
			else if (arg.length() >= 2)
			{
				if (arg.at(arg.length() - 1) == '.' || arg.at(arg.length() - 1) == ',') // 10. -> 10.00
				{
					arg.append(precision, '0');
				}
				else
				{
					uint8_t i = 0;
					while (i != arg.size() && arg.at(i) != '.' && arg.at(i) != ',')
					{
						++i;
					}
					if (i != arg.size())
					{
						++i; // skip '.' or ','
					}
					uint8_t fraction_digits = 0;
					while (i != arg.size())
					{
						++i;
						++fraction_digits;
					}
					if (fraction_digits < precision)
					{
						// [2] .1 -> 0.10
						// [3] .15 -> 0.150
						arg.append(precision - fraction_digits, '0');
					}
					else if (fraction_digits > precision)
					{
						// [2] .10000 -> 0.10
						const auto superfluous = (fraction_digits - precision);
						arg.erase(arg.size() - superfluous, superfluous);
					}
				}
			}
			StringUtils::replace_all(arg, L".", L"");
			StringUtils::replace_all(arg, L",", L"");
			if (!onCommandImplValue(click, std::move(arg)))
			{
				return;
			}
		}
		onCommandImplDispatch(click, prev_value);
	}

	std::string CommandSliderFloat::getState() const
	{
		return StringUtils::utf16_to_utf8(formatNumber(value, false));
	}

	std::string CommandSliderFloat::getDefaultState() const
	{
		return StringUtils::utf16_to_utf8(formatNumber(default_value, false));
	}

	void CommandSliderFloat::setValue(float value, Click& click)
{
		const auto scalar = getPrecisionScalar();
		return CommandSlider::setValue(click, (int)(value * scalar));
	}
}
