#pragma once

#include "CommandListWithOnTickAsActiveList.hpp"
#include "CommandList.hpp"

namespace Stand
{
	class CommandHistoricPlayerStats : public CommandListWithOnTickAsActiveList<CommandList>
	{
	private:
		RemoteStats* s = nullptr;

	public:
		explicit CommandHistoricPlayerStats(CommandList* const parent);

		void onClick(Click& click) final;

		void onActiveListUpdate() final;

		void onTickAsActiveList() final;
	};
}
