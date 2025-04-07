#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandPlayerHistory : public CommandList
	{
	public:
		CommandListHistoricPlayers* starred;
		CommandListHistoricPlayers* tracked;

		explicit CommandPlayerHistory();

		void onClick(Click& click) final;

		[[nodiscard]] bool hasCommandsWhereCommandNameStartsWith(const CommandName& command_name_prefix) const noexcept final;
	};
}
