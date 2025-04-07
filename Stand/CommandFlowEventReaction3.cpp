#include "CommandFlowEventReaction3.hpp"

#include <fmt/core.h>

#include "CommandFlowEvent.hpp"
#include "Hooking.hpp"
#include "lang.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandFlowEventReaction3::CommandFlowEventReaction3(CommandList* const parent, flowevent_t event, floweventreaction_t reaction, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
		: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text),
			{
				{2, LOC("ON")},
				{1, LOC("PS_S")},
				{0, LOC("DOFF")},
			}, // default value:
			(
				(((CommandFlowEvent*)parent)->getReactionData().getReactions(PlayerType::FRIEND) & reaction) ? 2 : (
					(((CommandFlowEvent*)parent)->getReactionData().getReactions(PlayerType::STRANGER) & reaction) ? 1 : 0
				)
			)
		, CMDFLAGS_LIST_SELECT | CMDFLAG_SEARCH_FINISHLIST | (((CommandFlowEvent*)parent)->shouldShowAllReactionsInFeatureList() ? parent->flags : parent->flags | CMDFLAG_FEATURELIST_FINISHLIST), reaction == REACTION_TOAST ? parent->flags : parent->flags | CMDFLAG_FEATURELIST_FINISHLIST), reaction(reaction)
	{
		this->flags |= get_reaction_flags(event, reaction);
	}

	void CommandFlowEventReaction3::setState(Click& click, const std::string& state)
	{
		// 0.46 -> 0.47
		if (state == "Exclude Friends")
		{
			return CommandListSelect::setState(click, "Strangers");
		}

		return CommandListSelect::setState(click, state);
	}

	void CommandFlowEventReaction3::onChange(Click& click, long long prev_value)
	{
		auto* parent = ((CommandFlowEvent*)this->parent);
		if (click.isMenu())
		{
			if (value != default_value
				&& reaction == REACTION_TIMEOUT
				&& parent->getEventType() == FlowEvent::SE_CRASH
				)
			{
				onChangeImplWarning(click, prev_value, LANG_GET_W("GENWARN"), WARNING_COMMAND_BOUND);
			}
		}
		if (value != 0)
		{
			if ((((flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_BASIC) && !click.isBasicEdition())
				|| (((flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_ULTIMATE) && !click.isUltimateEdition())
				)
			{
				return onChangeImplUnavailable(click, prev_value);
			}
			if (click.isMenu())
			{
				auto warning = parent->getBroadWarning(reaction);
				if (is_reaction_a_bad_idea(reaction, !warning.empty()))
				{
					if (warning.empty())
					{
						warning = LANG_GET_W("GENWARN");
					}
					onChangeImplWarning(click, prev_value, std::move(warning), WARNING_COMMAND_BOUND);
					return;
				}
			}
		}
		auto reaction_data = parent->getReactionData();
		if (value == 0)
		{
			reaction_data.getReactions(PlayerType::FRIEND) &= ~reaction;
			reaction_data.getReactions(PlayerType::STRANGER) &= ~reaction;
		}
		else
		{
			if (value == 1)
			{
				reaction_data.getReactions(PlayerType::FRIEND) &= ~reaction;
			}
			else
			{
				reaction_data.getReactions(PlayerType::FRIEND) |= reaction;
			}
			reaction_data.getReactions(PlayerType::STRANGER) |= reaction;
		}
		parent->setReactionData(std::move(reaction_data));
	}
}
