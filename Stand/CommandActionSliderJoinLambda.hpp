#pragma once

#include "CommandTextsliderJoin.hpp"

namespace Stand
{
	class CommandActionSliderJoinLambda : public CommandTextsliderJoin
	{
	public:
		const std::function<void(long long)> on_click_impl;

		explicit CommandActionSliderJoinLambda(CommandList* const parent, Label&& menu_name, std::function<void(long long)>&& on_click_impl, std::vector<CommandName>&& command_names = {}, commandflags_t extra_flags = 0)
			: CommandTextsliderJoin(parent, std::move(menu_name), std::move(command_names), extra_flags), on_click_impl(std::move(on_click_impl))
		{
		}

		void onClick(Click& click) final
		{
			on_click_impl(value);
		}
	};
}
