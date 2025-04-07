#pragma once

#include "CommandListHistoricPlayers.hpp"

namespace Stand
{
	class CommandMarkedPlayers : public CommandListHistoricPlayers
	{
	public:
		explicit CommandMarkedPlayers(CommandList* parent);

	protected:
		[[nodiscard]] std::vector<CommandHistoricPlayer*> collectPlayers() const final;
	};
}
