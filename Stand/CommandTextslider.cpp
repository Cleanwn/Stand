#include "CommandTextslider.hpp"

namespace Stand
{
	CommandTextslider::CommandTextslider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandTextsliderData>&& options, const commandflags_t flags, const CommandPerm perms, const CommandType type)
		: CommandPhysical(type, parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, perms), options(std::move(options))
	{
		resetValue();
	}

	bool CommandTextslider::hasOptions() const noexcept
	{
		return !options.empty();
	}

	bool CommandTextslider::hasOption(long long value) const noexcept
	{
		for (const auto& option : options)
		{
			if (option.value == value)
			{
				return true;
			}
		}
		return false;
	}

	void CommandTextslider::setOptions(std::vector<CommandTextsliderData>&& options)
	{
		this->options = std::move(options);
		if (!hasOption(value))
		{
			resetValue();
		}
	}

	void CommandTextslider::resetValue()
	{
		if (hasOptions())
		{
			value = options.at(0).value;
		}
	}

	Label CommandTextslider::getCurrentValueLabel() const
	{
		for (const auto& option : options)
		{
			if (option.value == value)
			{
				return option.label;
			}
		}
		return NOLABEL;
	}

	void CommandTextslider::onFocus(ThreadContext thread_context, Direction momentum)
	{
		if (momentum != TELEPORT)
		{
			resetValue();
		}
	}

	bool CommandTextslider::onLeft(Click& click, bool holding)
	{
		if (!job_queued && hasOptions())
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
						processValueChange(click);
					}
					else
					{
						if (holding)
						{
							return false;
						}
						value = options.back().value;
						processValueChange(click);
					}
					break;
				}
				prev = option.value;
			}
		}
		return true;
	}

	bool CommandTextslider::onRight(Click& click, bool holding)
	{
		if (!job_queued && hasOptions())
		{
			long long prev_value = value;
			bool next = false;
			for (const auto& option : options)
			{
				if (next)
				{
					value = option.value;
					processValueChange(click);
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
				value = options.at(0).value;
				processValueChange(click);
			}
		}
		return true;
	}

	void CommandTextslider::processValueChange(const Click& click) const
	{
		processVisualUpdate(false);
	}
}
