#pragma once

#include "CommandFlowEvent.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandHistoricPlayerJoinReactions : public CommandFlowEvent
	{
	public:
		explicit CommandHistoricPlayerJoinReactions(CommandList* parent);

		void populate() final;
		[[nodiscard]] FlowEventReactionData getReactionData() const final;
		void setReactionData(FlowEventReactionData&& reaction_data) const final;
	};
#pragma pack(pop)
}
