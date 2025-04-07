#pragma once

#include "CommandAction.hpp"

#include <fmt/xchar.h>

namespace Stand
{
	class CommandClockPreset : public CommandAction
	{
	private:
		const int hour;

	public:
		explicit CommandClockPreset(CommandList* parent, Label&& menu_name, int hour)
			: CommandAction(parent, std::move(menu_name)), hour(hour)
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return getCommandSyntaxImpl(fmt::format(L"time {}", hour));
		}

		void onClick(Click& click) final
		{
			click.ensureYieldableScriptThread([this]
			{
				TimeOfDay tod{};
				tod.hour = hour;
				parent->as<CommandClock>()->setTime(std::move(tod));
			});
		}
	};
}
