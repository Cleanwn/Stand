#pragma once

#include "CommandReadonlyValueCopy.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandReadonlyValueLambda : public CommandReadonlyValueCopy
	{
	private:
		const std::function<std::wstring()> on_tick_in_viewport_impl;

	public:
		explicit CommandReadonlyValueLambda(CommandList* const parent, Label&& label, std::function<std::wstring()>&& on_tick_in_viewport_impl, commandflags_t flags = 0);

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;
	};
#pragma pack(pop)
}
