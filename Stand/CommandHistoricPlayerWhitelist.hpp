#pragma once

#include "CommandToggleNoCorrelation.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandHistoricPlayerWhitelist : public CommandToggleNoCorrelation
	{
	public:
		explicit CommandHistoricPlayerWhitelist(CommandList* parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;

		void onTickInWebViewport() final;
		void onTickInGameViewport() final;
	};
#pragma pack(pop)
}
