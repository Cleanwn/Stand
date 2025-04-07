#pragma once

#include "CommandFlowEvent.hpp"

namespace Stand
{
	class CommandFlowEventGlobal : public CommandFlowEvent
	{
	private:
		const flowevent_t type;
	public:
		explicit CommandFlowEventGlobal(CommandList* const parent, const flowevent_t type, Label&& help_text = NOLABEL, commandflags_t flags = 0);
		explicit CommandFlowEventGlobal(CommandList* const parent, Label&& event_name, const flowevent_t type, Label&& help_text = NOLABEL);

		void populate() final;
		[[nodiscard]] FlowEventReactionData getReactionData() const final;
		void setReactionData(FlowEventReactionData&& reaction_data) const final;
		[[nodiscard]] bool shouldShowAllReactionsInFeatureList() const final;
		[[nodiscard]] std::wstring getBroadWarning(floweventreaction_t reaction) const final;
		[[nodiscard]] flowevent_t getEventType() const final;
	};
}
