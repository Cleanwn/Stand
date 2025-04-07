#pragma once

#include "CommandListHistoricPlayers.hpp"

namespace Stand
{
	class CommandTrackedPlayers : public CommandListHistoricPlayers
	{
	public:
		explicit CommandTrackedPlayers(CommandList* parent);

		void onClick(Click& click) final;

	protected:
		[[nodiscard]] std::vector<CommandHistoricPlayer*> collectPlayers() const final;
	};
}
