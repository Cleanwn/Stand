#include "CommandToggleEditionbound.hpp"

#include "get_current_time_millis.hpp"

namespace Stand
{
	void CommandToggleEditionbound::onChange(Click& click)
	{
		if (m_on == default_on)
		{
			onChangeInner(click);
		}
		else if (doPermissionCheck(click) && GET_MILLIS_SINCE(Click::last_successful_permission_check) <= 100)
		{
			onChangeInner(click);
		}
		else
		{
			onChangeImplUnavailable(click);
		}
	}

	void CommandToggleEditionbound::onChangeInner(Click& click)
	{
		if (m_on)
		{
			onEnableInner(click);
		}
		else
		{
			onDisableInner(click);
		}
	}

	void CommandToggleEditionbound::onEnableInner(Click& click)
	{
	}

	void CommandToggleEditionbound::onDisableInner(Click& click)
	{
	}
}
