#include "CommandColourSlider.hpp"

namespace Stand
{
	void CommandColourSlider::onCommand(Click& click, std::wstring& args)
	{
		// So "vehprimaryred" etc behave as expected when commands are not in view.
		if (!parent->receivesOnTickInGameViewportThisTick()
			&& !parent->isCurrentUiOrWebList()
			)
		{
			parent->onTickInGameViewport();
		}

		return CommandSlider::onCommand(click, args);
	}

	Label CommandColourSlider::getActivationName() const
	{
		return getActivationNameImplCombineWithParent();
	}

	void CommandColourSlider::onChange(Click& click, int prev_value)
	{
		if (click.type != CLICK_BULK)
		{
			auto* const parent = (CommandColour*)this->parent;
			switch (value_type)
			{
			case RGB:
				parent->updateHSV();
				break;

			case HSV:
				parent->updateRGB();
				break;
			}
			parent->processChange(click);
		}
	}
}
