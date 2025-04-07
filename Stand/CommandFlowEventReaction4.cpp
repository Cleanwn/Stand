#include "CommandFlowEventReaction4.hpp"

#include <fmt/core.h>

#include "CommandFlowEvent.hpp"
#include "Hooking.hpp"
#include "lang.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandFlowEventReaction4::CommandFlowEventReaction4(CommandList* const parent, flowevent_t event, floweventreaction_t reaction, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
		: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text),
			{
				{3, LOC("ON"), {}, LOC("PS_ON_H")},
				{2, LOC("PS_F_S")},
				{1, LOC("PS_S")},
				{0, LOC("DOFF")},
			}, // default value:
			(
				(((CommandFlowEvent*)parent)->getReactionData().getReactions(PlayerType::SELF) & reaction) ? 3 : (
					(((CommandFlowEvent*)parent)->getReactionData().getReactions(PlayerType::FRIEND) & reaction) ? 2 : (
						(((CommandFlowEvent*)parent)->getReactionData().getReactions(PlayerType::STRANGER) & reaction) ? 1 : 0
					)
				)
			)
		, CMDFLAGS_LIST_SELECT | CMDFLAG_SEARCH_FINISHLIST | (((CommandFlowEvent*)parent)->shouldShowAllReactionsInFeatureList() ? parent->flags : parent->flags | CMDFLAG_FEATURELIST_FINISHLIST), reaction == REACTION_TOAST ? parent->flags : parent->flags | CMDFLAG_FEATURELIST_FINISHLIST), reaction(reaction)
	{
		this->flags |= get_reaction_flags(event, reaction);
	}

	void CommandFlowEventReaction4::onChange(Click& click, long long prev_value)
	{
		auto* parent = ((CommandFlowEvent*)this->parent);
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
		if (value < 3)
		{
			reaction_data.getReactions(PlayerType::SELF) &= ~reaction;
		}
		else
		{
			reaction_data.getReactions(PlayerType::SELF) |= reaction;
		}
		if (value < 2)
		{
			reaction_data.getReactions(PlayerType::FRIEND) &= ~reaction;
		}
		else
		{
			reaction_data.getReactions(PlayerType::FRIEND) |= reaction;
		}
		if (value < 1)
		{
			reaction_data.getReactions(PlayerType::STRANGER) &= ~reaction;
		}
		else
		{
			reaction_data.getReactions(PlayerType::STRANGER) |= reaction;
		}
		parent->setReactionData(std::move(reaction_data));
	}
}
