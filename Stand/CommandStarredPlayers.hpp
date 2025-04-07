#pragma once

#include "CommandListHistoricPlayers.hpp"

namespace Stand
{
	class CommandStarredPlayers : public CommandListHistoricPlayers
	{
	public:
		explicit CommandStarredPlayers(CommandList* parent);

	protected:
		[[nodiscard]] std::vector<CommandHistoricPlayer*> collectPlayers() const final;
	};
}
