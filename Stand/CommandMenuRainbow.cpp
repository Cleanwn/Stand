#include "CommandMenuRainbow.hpp"

#include "ColourUtil.hpp"
#include "MenuGrid.hpp"

namespace Stand
{
	void CommandMenuRainbow::onHueChange(CommandRainbow* _thisptr)
	{
		auto* const thisptr = (CommandMenuRainbow*)_thisptr;
		if (!ColourUtil::isContrastSufficient(thisptr->colour->getRGB(), thisptr->focusTextColour->getRGB()))
		{
			DirectX::SimpleMath::Color colour = thisptr->focusTextColour->getRGBA();
			colour.Negate();
			Click click(CLICK_AUTO, TC_RENDERER);
			thisptr->focusTextColour->set(click, colour);
			g_menu_grid.update();
		}
		if (!ColourUtil::isContrastSufficient(thisptr->colour->getRGB(), thisptr->focusRightTextColour->getRGB()))
		{
			DirectX::SimpleMath::Color colour = thisptr->focusRightTextColour->getRGBA();
			colour.Negate();
			Click click(CLICK_AUTO, TC_RENDERER);
			thisptr->focusRightTextColour->set(click, colour);
			g_menu_grid.update();
		}
		if (!ColourUtil::isContrastSufficient(thisptr->colour->getRGB(), thisptr->focusSpriteColour->getRGB()))
		{
			DirectX::SimpleMath::Color colour = thisptr->focusSpriteColour->getRGBA();
			colour.Negate();
			Click click(CLICK_AUTO, TC_RENDERER);
			thisptr->focusSpriteColour->set(click, colour);
			g_menu_grid.update();
		}
	}
}
