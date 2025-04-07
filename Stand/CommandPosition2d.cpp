#include "CommandPosition2d.hpp"

#include <fmt/format.h>

#include "CommandLambdaSlider.hpp"
#include "CommandPosition2dMouse.hpp"
#include "CommandSlider.hpp"
#include "CommandToggle.hpp"
#include "lang.hpp"

namespace Stand
{
	CommandPosition2d::CommandPosition2d(CommandList* parent, Label&& menu_name, const CommandName& command_name, const Position2d& default_pos, Label&& help_text)
		: CommandList(parent, std::move(menu_name), { std::move(CommandName(command_name).append(CMDNAME("pos"))) }, std::move(help_text), CMDFLAGS_LIST | CMDFLAG_SUPPORTS_STATE_OPERATIONS)
	{
		if (mustHaveCommandName())
		{
			return;
		}
		x = createChild<CommandLambdaSlider<CommandSlider>>(LIT("X"), { std::move(CommandName(command_name).append(CMDNAME("x"))) }, NOLABEL, SHRT_MIN, SHRT_MAX, default_pos.x, 10, [this](int value, Click& click)
		{
			this->onChange();
		}, CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
		y = createChild<CommandLambdaSlider<CommandSlider>>(LIT("Y"), { std::move(CommandName(command_name).append(CMDNAME("y"))) }, NOLABEL, SHRT_MIN, SHRT_MAX, default_pos.y, 10, [this](int value, Click& click)
		{
			this->onChange();
		}, CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
		mouse = createChild<CommandPosition2dMouse>(command_name);
	}

	Position2d CommandPosition2d::getPosition() const
	{
		return { int16_t(x->value), int16_t(y->value) };
	}

	void CommandPosition2d::setPosition(Position2d&& pos) const
	{
		Click click(CLICK_AUTO, TC_OTHER);
		x->setValue(click, pos.x);
		y->setValue(click, pos.y);
		onChange(std::move(pos));
	}

	void CommandPosition2d::onChange() const
	{
		return onChange(getPosition());
	}

	void CommandPosition2d::onBack(ThreadContext thread_context)
	{
		ensureMouseMoveIsDisabled(thread_context);
	}

	std::string CommandPosition2d::getState() const
	{
		if (x->value == x->default_value
			&& y->value == y->default_value)
		{
			return "";
		}
		return fmt::format("{}, {}", x->value, y->value);
	}

	void CommandPosition2d::setState(Click& click, const std::string& state)
	{
		auto i = state.find(", ");
		if (i == std::string::npos)
		{
			return;
		}
		x->setState(click, state.substr(0, i));
		y->setState(click, state.substr(i + 2));
	}

	void CommandPosition2d::applyDefaultState()
	{
		x->applyDefaultState();
		y->applyDefaultState();
		ensureMouseMoveIsDisabled(TC_APPLYDEFAULTSTATE);
	}

	void CommandPosition2d::ensureMouseMoveIsDisabled(ThreadContext thread_context)
	{
		Click click(CLICK_AUTO, thread_context);
		mouse->setStateBool(click, false);
	}
}
