#include "CommandFlowEventReaction2.hpp"

#include <fmt/core.h>

#include "CommandFlowEvent.hpp"
#include "Hooking.hpp"
#include "lang.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandFlowEventReaction2::CommandFlowEventReaction2(CommandList* const parent, flowevent_t event, floweventreaction_t reaction, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
		: CommandToggle(parent, std::move(menu_name), std::move(command_names), std::move(help_text), ((CommandFlowEvent*)parent)->getReactionData().getReactions() & reaction, CMDFLAGS_TOGGLE | CMDFLAG_SEARCH_FINISHLIST | (((CommandFlowEvent*)parent)->shouldShowAllReactionsInFeatureList() ? parent->flags : parent->flags | CMDFLAG_FEATURELIST_FINISHLIST)), reaction(reaction)
	{
		this->flags |= get_reaction_flags(event, reaction);
	}

	void CommandFlowEventReaction2::onChange(Click& click)
	{
		auto* parent = (CommandFlowEvent*)this->parent;
		if (m_on)
		{
			if ((((flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_BASIC) && !click.isBasicEdition())
				|| (((flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_ULTIMATE) && !click.isUltimateEdition())
				)
			{
				return onChangeImplUnavailable(click);
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
					setOnIndication(false);
					showWarning(click, std::move(warning), WARNING_COMMAND_BOUND, [this](ThreadContext thread_context)
					{
						Click click(CLICK_AUTO, thread_context);
						setStateBool(click, true);
					});
					return;
				}
			}
		}
		auto reaction_data = parent->getReactionData();
		if (m_on)
		{
			reaction_data.getReactions(PlayerType::SELF) |= reaction;
			reaction_data.getReactions(PlayerType::FRIEND) |= reaction;
			reaction_data.getReactions(PlayerType::STRANGER) |= reaction;
		}
		else
		{
			reaction_data.getReactions(PlayerType::SELF) &= ~reaction;
			reaction_data.getReactions(PlayerType::FRIEND) &= ~reaction;
			reaction_data.getReactions(PlayerType::STRANGER) &= ~reaction;
		}
		parent->setReactionData(std::move(reaction_data));
	}
}
