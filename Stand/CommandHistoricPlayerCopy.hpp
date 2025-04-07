#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandHistoricPlayerCopy : public CommandToggle
	{
	public:
		explicit CommandHistoricPlayerCopy(CommandList* parent, const CommandName& simple_name);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;
	};
#pragma pack(pop)
}
