#pragma once

#include "CommandSlider.hpp"

namespace Stand
{
	class CommandSliderDynamic : public CommandSlider
	{
	protected:
		explicit CommandSliderDynamic(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const int skip = 1);

	public:
		void onPreScriptedAccess() final;

		void onCommand(Click& click, std::wstring& args) final;

		void onTickInGameViewport() override = 0;
		void onTickInWebViewport() final;
	};
}
