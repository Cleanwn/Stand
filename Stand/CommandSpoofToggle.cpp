#include "CommandSpoofToggle.hpp"

#include "CommandSpoof.hpp"
#include "CommandSlider.hpp"
#include "is_session.hpp"

namespace Stand
{
	void CommandSpoofToggle::onChange(Click& click)
	{
		reinterpret_cast<CommandSpoof<CommandSlider>*>(parent)->onToggle(m_on);
		CommandToggle::onChange(click);
	}

	void CommandSpoofToggle::onEnable(Click& click)
	{
		if (!click.isBasicEdition())
		{
			return onChangeImplUnavailable(click);
		}
		registerAsyncTickEventHandler(click, [this]
		{
			if (!m_on)
			{
				return false;
			}
			if (is_session_started_and_transition_finished())
			{
				reinterpret_cast<CommandSpoof<CommandSlider>*>(parent)->onTick();
			}
			return true;
		});
	}
}
