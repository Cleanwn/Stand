#pragma once

#include "CommandList.hpp"

#include "CommandDivider.hpp"
#include "FlowEvent.hpp"
#include "FlowEventReaction.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandFlowEvent : public CommandList
	{
	public:
		std::vector<CommandName> block_join_command_names{};

		explicit CommandFlowEvent(CommandList* const parent, Label&& event_name, Label&& help_text = NOLABEL, commandflags_t flags = 0);

		void populate(flowevent_t event);
	private:
		[[nodiscard]] static Label getBlockReactionHelpText(flowevent_t event) noexcept;
		void addPlayerEventReaction(flowevent_t event, floweventreaction_t reaction, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = {});

		template <typename T>
		void addReactionsForEventWithPlayer(flowevent_t event)
		{
			createChild<CommandDivider>(LOC("RTC_PLY"));
			if (FlowEvent::can_react_aggressively(event))
			{
				if (FlowEvent::kick_is_block_join(event))
				{
					createChild<T>(event, REACTION_KICK, LOC("BJ"), std::move(block_join_command_names), LOC("BJ_H"));
				}
				else
				{
					createChild<T>(event, REACTION_KICK, LOC("PLYKCK"));
					createChild<T>(event, REACTION_LL_KICK, LOC("PLYKCK_L_G"), CMDNAMES(), Label::combineWithSpace(LOC("PLYKCK_H_DSCRT"), LOC("PLYKCK_H_B_H")));
					createChild<T>(event, REACTION_BLACKLIST_KICK, LOC("PLYKCK_BL_A"), CMDNAMES(), Label::combineWithSpace(LOC("PLYKCK_BL_H"), LOC("GEN_H_HOST")));
				}
				createChild<T>(event, REACTION_CRASH, LOC("PLYCRSH"));
			}
			if (FlowEvent::can_react_with_timeout(event))
			{
				createChild<T>(event, REACTION_TIMEOUT, LOC("TIMEOUT"), CMDNAMES(), LOC("TIMEOUT_H"));
			}
			createChild<T>(event, REACTION_NET_BLOCK, LOC("RCT_N_B"));
			createChild<T>(event, REACTION_SYNCIN_BLOCK, LOC("RCT_BIS"));
			createChild<T>(event, REACTION_SYNCOUT_BLOCK, LOC("RCT_BOS"));
			createChild<T>(event, REACTION_NET_LOG, LOC("RCT_N_L"));
			createChild<T>(event, REACTION_SYNCIN_LOG, LOC("RCT_SI_L"));
			createChild<T>(event, REACTION_SYNCOUT_LOG, LOC("RCT_SO_L"));
			createChild<T>(event, REACTION_NET_TOAST, LOC("RCT_N_T"));
			createChild<T>(event, REACTION_SYNCIN_TOAST, LOC("RCT_SI_T"));
			createChild<T>(event, REACTION_SYNCOUT_TOAST, LOC("RCT_SO_T"));
		}

	public:
		virtual void populate() = 0;

		[[nodiscard]] virtual FlowEventReactionData getReactionData() const = 0;
		virtual void setReactionData(FlowEventReactionData&& reaction_data) const = 0;
		[[nodiscard]] virtual bool shouldShowAllReactionsInFeatureList() const;
		[[nodiscard]] virtual std::wstring getBroadWarning(floweventreaction_t reaction) const;
		[[nodiscard]] virtual flowevent_t getEventType() const;
	};
#pragma pack(pop)
}
