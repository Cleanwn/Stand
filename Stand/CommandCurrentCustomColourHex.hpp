#pragma once

#include "CommandReadonlyValueCopy.hpp"

#include "CommandColourCustom.hpp"

namespace Stand
{
	class CommandCurrentCustomColourHex : public CommandReadonlyValueCopy
	{
	private:
		CommandColourCustom* const colour = nullptr;

	public:
		explicit CommandCurrentCustomColourHex(CommandList* const parent, CommandColourCustom* const colour)
			: CommandReadonlyValueCopy(parent, LOC("CURRCLRHEX"), {}, CMDFLAGS_READONLY_VALUE_COPY | CMDFLAG_FEATURELIST_SKIP), colour(colour)
		{
		}

		void onTickInGameViewport() final
		{
			setValue(colour->getHex());
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void onPreScriptedAccess() final
		{
			return dispatchOnTickInGameViewportIfNotThisTick();
		}
	};
}