#include "CommandFlowEvent.hpp"

#include "CommandFlowEventReaction4.hpp"
#include "CommandFlowEventReaction3.hpp"
#include "CommandFlowEventReaction2.hpp"
#include "FlowEvent.hpp"
#include "lang.hpp"

namespace Stand
{
	CommandFlowEvent::CommandFlowEvent(CommandList* const parent, Label&& event_name, Label&& help_text, commandflags_t flags)
		: CommandList(parent, std::move(event_name), {}, std::move(help_text), flags)
	{
	}

	void CommandFlowEvent::populate(flowevent_t event)
	{
		if (event == FlowEvent::NE_ANY)
		{
			flags |= CMDFLAG_FEATURELIST_FINISHLIST;
		}
		if (FlowEvent::can_discriminate(event))
		{
			if (FlowEvent::can_notify(event))
			{
				addPlayerEventReaction(event, REACTION_TOAST, LOC("RCT_T"));
			}
			if (FlowEvent::can_be_blocked(event))
			{
				if (FlowEvent::can_be_blocked_from_self(event))
				{
					createChild<CommandFlowEventReaction4>(event, REACTION_BLOCK, LOC("PTX_NET_R_B"), CMDNAMES(), getBlockReactionHelpText(event));
				}
				else
				{
					createChild<CommandFlowEventReaction3>(event, REACTION_BLOCK, LOC("PTX_NET_R_B"), CMDNAMES(), getBlockReactionHelpText(event));
				}
			}
			if (!FlowEvent::is_broad(event))
			{
				addPlayerEventReaction(event, REACTION_LOG_FILE, LOC("RCT_L_F"));
				addPlayerEventReaction(event, REACTION_LOG_CONSOLE, LOC("RCT_L_C"));
				addPlayerEventReaction(event, REACTION_CHAT, LOC("RCT_C"));
				addPlayerEventReaction(event, REACTION_CHAT_TEAM, LOC("RCT_C_T"));
				addReactionsForEventWithPlayer<CommandFlowEventReaction3>(event);
			}
			else
			{
				if (FlowEvent::can_notify(event))
				{
					addPlayerEventReaction(event, REACTION_LOG_FILE, LOC("RCT_L_F"));
					addPlayerEventReaction(event, REACTION_LOG_CONSOLE, LOC("RCT_L_C"));
				}
			}
		}
		else
		{
			if (FlowEvent::can_notify(event))
			{
				createChild<CommandFlowEventReaction2>(event, REACTION_TOAST, LOC("RCT_T"));
			}
			if (FlowEvent::can_be_blocked(event))
			{
				createChild<CommandFlowEventReaction2>(event, REACTION_BLOCK, LOC("PTX_NET_R_B"), CMDNAMES(), getBlockReactionHelpText(event));
			}
			if (FlowEvent::can_notify(event))
			{
				createChild<CommandFlowEventReaction2>(event, REACTION_LOG_FILE, LOC("RCT_L_F"));
				createChild<CommandFlowEventReaction2>(event, REACTION_LOG_CONSOLE, LOC("RCT_L_C"));
			}
			if (!FlowEvent::is_broad(event))
			{
				createChild<CommandFlowEventReaction2>(event, REACTION_CHAT, LOC("RCT_C"));
				createChild<CommandFlowEventReaction2>(event, REACTION_CHAT_TEAM, LOC("RCT_C_T"));
			}
			if (FlowEvent::has_player(event))
			{
				addReactionsForEventWithPlayer<CommandFlowEventReaction2>(event);
			}
		}
	}

	Label CommandFlowEvent::getBlockReactionHelpText(flowevent_t event) noexcept
	{
		if (FlowEvent::is_modder_detection(event))
		{
			std::string str;
			if (event == FlowEvent::MOD_EXPBLAME || event == FlowEvent::MOD_BADEXPLOSION)
			{
				str = LANG_GET("MDDRT_EXPL_B");
			}
			return LIT(std::move(str));
		}
		if (FlowEvent::is_chat_message(event))
		{
			return LOC("PTX_NET_R_B_H_C");
		}
		return NOLABEL;
	}

	void CommandFlowEvent::addPlayerEventReaction(flowevent_t event, floweventreaction_t reaction, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
	{
		if (FlowEvent::can_be_self(event))
		{
			createChild<CommandFlowEventReaction4>(event, reaction, std::move(menu_name), std::move(command_names), std::move(help_text));
		}
		else
		{
			createChild<CommandFlowEventReaction3>(event, reaction, std::move(menu_name), std::move(command_names), std::move(help_text));
		}
	}

	bool CommandFlowEvent::shouldShowAllReactionsInFeatureList() const
	{
		return false;
	}

	std::wstring CommandFlowEvent::getBroadWarning(floweventreaction_t reaction) const
	{
		return {};
	}

	flowevent_t CommandFlowEvent::getEventType() const
	{
		return -1;
	}
}
