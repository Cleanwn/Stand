#pragma once

#include "CommandListHistoricPlayers.hpp"

namespace Stand
{
	class CommandWhitelistedPlayers : public CommandListHistoricPlayers
	{
	public:
		explicit CommandWhitelistedPlayers(CommandList* parent);

	protected:
		[[nodiscard]] std::vector<CommandHistoricPlayer*> collectPlayers() const final;
	};
}
