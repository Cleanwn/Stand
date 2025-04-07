#pragma once

#include "CommandReadonlyValueCopy.hpp"

namespace Stand
{
	class CommandPlayerIntel : public CommandReadonlyValueCopy
	{
	public:
		explicit CommandPlayerIntel(CommandList* const parent, Label&& menu_name, const commandflags_t flags = CMDFLAGS_READONLY_VALUE_COPY)
			: CommandReadonlyValueCopy(parent, std::move(menu_name), std::wstring{}, flags)
		{
		}

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;

		void onPreScriptedAccess() final;

	protected:
		virtual std::wstring getValue(const AbstractPlayer p) const = 0;
	};
}
