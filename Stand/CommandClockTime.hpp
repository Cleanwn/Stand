#pragma once

#include "CommandInput.hpp"

#include <fmt/core.h>

#include "get_next_arg.hpp"

namespace Stand
{
	class CommandClockTime : public CommandInput
	{
	public:
		explicit CommandClockTime(CommandList* const parent)
			: CommandInput(parent, LOC("TIME"), CMDNAMES("time"), NOLABEL, {}, CMDFLAGS_ACTION_VALUE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

	private:
		void updateValue()
		{
			setValue(fmt::format("{:0>2}:{:0>2}:{:0>2}", CLOCK::GET_CLOCK_HOURS(), CLOCK::GET_CLOCK_MINUTES(), CLOCK::GET_CLOCK_SECONDS()));
		}

	public:
		void onTickInGameViewport() final
		{
			updateValue();
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto arg = get_next_arg(args);
			if (arg.empty())
			{
				if (click.canShowCommandBox())
				{
					ensureScriptThread(click, [this, click](ThreadContext thread_context)
					{
						Click click_(click);
						click_.thread_context = thread_context;
						updateValue();
						onClick(click_);
					});
				}
				return;
			}
			auto tod = TimeOfDay::fromString(std::move(arg));
			if (!tod.isValid())
			{
				click.setResponse(LOC("INVARG"));
				return;
			}
			ensureYieldableScriptThread(click, [this, tod{ std::move(tod) }]() mutable
			{
				parent->as<CommandClock>()->setTime(std::move(tod));
			});
		}
	};
}
