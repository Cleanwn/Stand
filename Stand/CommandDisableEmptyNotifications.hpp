#pragma once

#include "CommandToggle.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandDisableEmptyNotifications : public CommandToggle
	{
	public:
		explicit CommandDisableEmptyNotifications(CommandList* const parent)
			: CommandToggle(parent, LOC("NEMTYNTFY"), combineCommandNames(CMDNAMES("no", "disable", "block"), CMDNAMES("emptynotifications", "emptynotifys")), NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			g_hooking.flow_event_reactions[FlowEvent::SE_NOTIFY_EMPTY] = (m_on * REACTION_BLOCK);
		}
	};
}
