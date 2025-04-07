#pragma once

#include "CommandSliderFloat.hpp"

namespace Stand
{
	template <float v3::* axis_offset>
	class CommandVector3Slider : public CommandSliderFloat
	{
	public:
		explicit CommandVector3Slider(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names)
			: CommandSliderFloat(parent, std::move(menu_name), std::move(command_names), NOLABEL, -1000000, 1000000, 0, 200, CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

		void onTickInGameViewport() final
		{
			auto vec = parent->as<CommandVector3>()->getVec();
			Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
			setValue(vec.*axis_offset, click);
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void onChange(Click& click, int prev_value) final
		{
			if (!click.isStand())
			{
				auto vec = parent->as<CommandVector3>()->getVec();
				vec.*axis_offset = getFloatValue();
				parent->as<CommandVector3>()->setVec(std::move(vec));
			}
		}
	};
}
