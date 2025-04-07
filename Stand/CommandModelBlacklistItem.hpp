#pragma once

#include "CommandFlowEvent.hpp"

#include "CommandLambdaAction.hpp"
#include "Hooking.hpp"
#include "joaatToString.hpp"

namespace Stand
{
	class CommandModelBlacklistItem : public CommandFlowEvent
	{
	public:
		const hash_t hash;

		explicit CommandModelBlacklistItem(CommandList* parent, const hash_t hash)
			: CommandFlowEvent(parent, AbstractModel(hash).getLabel(), NOLABEL, (CMDFLAGS_FLOW_EVENT & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_TEMPORARY | CMDFLAG_STATE_AND_CHECK_FINISHLIST | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_SKIP), hash(hash)
		{
			auto hashKey = joaatToString(hash);
			if (!menu_name.isLiteral()
				|| hashKey != menu_name.getLiteralUtf8()
				)
			{
				help_text.setLiteral(hashKey);
			}

			this->createChild<CommandLambdaAction>(LOC("DEL"), {}, NOLABEL, [this](Click& click)
			{
				this->parent->as<CommandModelBlacklist>()->removeItem(Util::to_padded_hex_string(this->hash));
			});
			this->createChild<CommandDivider>(LOC("RCTS"));
		}

		void populate() final
		{
			return CommandFlowEvent::populate(FlowEvent::MISC_MODELBLACKLIST);
		}

		FlowEventReactionData getReactionData() const final
		{
			return g_hooking.custom_model_sync_reactions.at(hash);
		}

		void setReactionData(FlowEventReactionData&& reaction_data) const final
		{
			g_hooking.custom_model_sync_reactions.at(hash) = std::move(reaction_data);
		}
	};
}
