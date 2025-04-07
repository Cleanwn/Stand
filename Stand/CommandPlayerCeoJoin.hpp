#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerCeoJoin : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerCeoJoin(CommandList* const parent);

		void onCommand(Click& click, std::wstring& args) final;

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;
	};
}
