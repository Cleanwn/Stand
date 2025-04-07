#include "CommandDivider.hpp"

#include "CommandList.hpp"
#include "Gui.hpp"

namespace Stand
{
	CommandDivider::CommandDivider(CommandList* const parent, Label&& menu_name)
		: CommandPhysical(COMMAND_DIVIDER, parent, std::move(menu_name), {}, NOLABEL, CMDFLAG_FEATURELIST_SKIP)
	{
		if (parent != nullptr)
		{
			++parent->dividers;
		}
	}

	void CommandDivider::preDetach()
	{
		CommandPhysical::preDetach();
		--parent->dividers;
	}

	void CommandDivider::onFocus(ThreadContext thread_context, Direction momentum)
	{
		if (!selectable)
		{
			if (g_gui.getCurrentUiList()->canUpdateCursor())
			{
				if (momentum == UP)
				{
					g_gui.inputUp(thread_context);
				}
				else
				{
					g_gui.inputDown(thread_context);
				}
			}
		}
	}
}
