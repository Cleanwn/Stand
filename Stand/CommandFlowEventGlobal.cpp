#include "CommandFlowEventGlobal.hpp"

#include "FlowEvent.hpp"
#include "Hooking.hpp"
#include "lang.hpp"

namespace Stand
{
	CommandFlowEventGlobal::CommandFlowEventGlobal(CommandList* const parent, const flowevent_t type, Label&& help_text, commandflags_t flags)
		: CommandFlowEvent(parent, FlowEvent::getName(type), std::move(help_text), flags), type(type)
	{
	}

	CommandFlowEventGlobal::CommandFlowEventGlobal(CommandList* const parent, Label&& event_name, const flowevent_t type, Label&& help_text)
		: CommandFlowEvent(parent, std::move(event_name), std::move(help_text)), type(type)
	{
	}

	void CommandFlowEventGlobal::populate()
	{
		return CommandFlowEvent::populate(type);
	}

	FlowEventReactionData CommandFlowEventGlobal::getReactionData() const
	{
		return g_hooking.flow_event_reactions[type];
	}

	void CommandFlowEventGlobal::setReactionData(FlowEventReactionData&& reaction_data) const
	{
		g_hooking.flow_event_reactions[type] = std::move(reaction_data);
	}

	bool CommandFlowEventGlobal::shouldShowAllReactionsInFeatureList() const
	{
		return FlowEvent::should_show_all_reactions_in_feature_list(type);
	}

	std::wstring CommandFlowEventGlobal::getBroadWarning(floweventreaction_t reaction) const
	{
		if (FlowEvent::is_broad(type))
		{
			if (reaction == REACTION_BLOCK && (type == FlowEvent::NE_ANY || type == SCRIPTED_GAME_EVENT || type == REQUEST_CONTROL_EVENT))
			{
				return LANG_GET_W("PTX_NET_R_B_W");
			}
			return LANG_GET_W("GENWARN");
		}
		return {};
	}

	flowevent_t CommandFlowEventGlobal::getEventType() const
	{
		return type;
	}
}
