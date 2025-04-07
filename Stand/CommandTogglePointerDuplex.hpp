#pragma once

#include "CommandTogglePointer.hpp"

namespace Stand
{
	class CommandTogglePointerDuplex : public CommandTogglePointer
	{
	public:
		using CommandTogglePointer::CommandTogglePointer;

		void onTickInGameViewport() final
		{
			setOnIndication(*ptr);
		}

		void onTickInWebViewport() final
		{
			onTickInWebViewportImplRedirect();
		}
	};
}
