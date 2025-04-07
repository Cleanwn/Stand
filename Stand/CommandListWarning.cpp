#include "CommandListWarning.hpp"

#include "Gui.hpp"
#include "Auth.hpp"

namespace Stand
{
	void CommandListWarning::onClick(Click& click)
	{
		if (m_warning_acknowledged || !shouldShowWarning())
		{
			CommandList::onClick(click);
		}
		else
		{
			showWarning(click, Lang::getW(m_warning), WARNING_COMMAND_BOUND, [=](ThreadContext thread_context) mutable
			{
				m_warning_acknowledged = true;
				click.thread_context = thread_context;
				onClick(click);
				if (!g_gui.on_command_state_change())
				{
					g_gui.active_profile.data[getPathConfig()] = "Warning Acknowledged";
					g_gui.active_profile.save();
				}
			});
		}
	}

	bool CommandListWarning::shouldShowWarning() const
	{
		return true;
	}
}
