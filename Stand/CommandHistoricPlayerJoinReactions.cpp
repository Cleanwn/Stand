#include "CommandHistoricPlayerJoinReactions.hpp"

#include "CommandHistoricPlayer.hpp"
#include "FlowEvent.hpp"
#include "FlowEventReaction.hpp"
#include "PlayerHistory.hpp"

namespace Stand
{
	CommandHistoricPlayerJoinReactions::CommandHistoricPlayerJoinReactions(CommandList* parent)
		: CommandFlowEvent(parent, LOC("JOINR"), NOLABEL, CMDFLAGS_FLOW_EVENT | CMDFLAG_TEMPORARY)
	{
	}

	void CommandHistoricPlayerJoinReactions::populate()
	{
		return CommandFlowEvent::populate(FlowEvent::MISC_HISTORYJOIN);
	}

	FlowEventReactionData CommandHistoricPlayerJoinReactions::getReactionData() const
	{
		return FlowEventReactionData(((CommandHistoricPlayer*)parent)->player->join_reactions);
	}

	void CommandHistoricPlayerJoinReactions::setReactionData(FlowEventReactionData&& reaction_data) const
	{
		((CommandHistoricPlayer*)parent)->player->join_reactions = reaction_data.reactions[0];
		PlayerHistory::needs_to_be_saved = true;
	}
}
