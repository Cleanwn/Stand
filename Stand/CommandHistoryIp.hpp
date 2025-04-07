#pragma once

#include "CommandReadonlyValueCopy.hpp"

namespace Stand
{
	class CommandHistoryIp : public CommandReadonlyValueCopy
	{
	private:
		HistoricPlayer* const hp;

	public:
		explicit CommandHistoryIp(CommandList* const parent, HistoricPlayer* const hp);

		void onTickInGameViewport() final;
	};
}
