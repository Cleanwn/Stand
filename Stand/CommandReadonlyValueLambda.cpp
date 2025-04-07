#include "CommandReadonlyValueLambda.hpp"

#include "CommandListReadonly.hpp"

namespace Stand
{
	CommandReadonlyValueLambda::CommandReadonlyValueLambda(CommandList* const parent, Label&& label, std::function<std::wstring()>&& on_tick_in_viewport_impl, commandflags_t flags)
		: CommandReadonlyValueCopy(parent, std::move(label), std::wstring{}, flags), on_tick_in_viewport_impl(std::move(on_tick_in_viewport_impl))
	{
	}

	void CommandReadonlyValueLambda::onTickInGameViewport()
	{
		if (parent->type == COMMAND_LIST_READONLY && !((CommandListReadonly*)parent)->had_initial_tick)
		{
			return;
		}
		setValue(on_tick_in_viewport_impl());
	}

	void CommandReadonlyValueLambda::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
