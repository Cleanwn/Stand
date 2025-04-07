#include "CommandSliderDynamic.hpp"

namespace Stand
{
	CommandSliderDynamic::CommandSliderDynamic(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const int skip)
		: CommandSlider(parent, std::move(menu_name), std::move(command_names), std::move(help_text), min_value, max_value, default_value, skip, CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
	{
	}

	void CommandSliderDynamic::onPreScriptedAccess()
	{
		if (!isInViewport())
		{
			onTickInGameViewport();
		}
	}

	void CommandSliderDynamic::onCommand(Click& click, std::wstring& args)
	{
		onPreScriptedAccess();
		return CommandSlider::onCommand(click, args);
	}

	void CommandSliderDynamic::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
