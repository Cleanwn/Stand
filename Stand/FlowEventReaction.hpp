#pragma once

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "Label.hpp"
#include "PlayerType.hpp"

// Please preserve order for player history. Cheers.

#define REACTION_TOAST				0b1
#define REACTION_BLOCK				0b10
#define REACTION_LL_KICK			0b100
#define REACTION_LOG_FILE			0b1000
#define REACTION_CHAT				0b10000
#define REACTION_CHAT_TEAM			0b100000
#define REACTION_KICK				0b1000000
#define REACTION_NET_BLOCK			0b10000000
#define REACTION_SYNCIN_BLOCK		0b100000000
#define REACTION_SYNCOUT_BLOCK		0b1000000000
#define REACTION_NET_LOG			0b10000000000
#define REACTION_SYNCIN_LOG			0b100000000000
#define REACTION_SYNCOUT_LOG		0b1000000000000
#define REACTION_NET_TOAST			0b10000000000000
#define REACTION_SYNCIN_TOAST		0b100000000000000
#define REACTION_SYNCOUT_TOAST		0b1000000000000000
#define REACTION_CRASH				0b10000000000000000
#define REACTION_TIMEOUT			0b100000000000000000
#define REACTION_BLACKLIST_KICK		0b1000000000000000000
#define REACTION_LOG_CONSOLE		0b10000000000000000000

#define REACTIONS_AGGRESSIVE (REACTION_LL_KICK | REACTION_KICK | REACTION_CRASH | REACTION_BLACKLIST_KICK)

namespace Stand
{
	[[nodiscard]] extern commandflags_t get_reaction_flags(flowevent_t event, floweventreaction_t reaction);
	[[nodiscard]] extern toast_t flow_event_reactions_to_logger_toast_flags(floweventreaction_t& reactions);
	[[nodiscard]] extern toast_t flow_event_reactions_to_toast_flags(floweventreaction_t& reactions);
	[[nodiscard]] extern floweventreaction_t toast_flags_to_flow_event_reactions(toast_t tf);
	[[nodiscard]] extern bool is_reaction_a_bad_idea(floweventreaction_t reaction, bool broad_event);

	[[nodiscard]] extern std::string reaction_to_string(floweventreaction_t reaction);
	[[nodiscard]] extern floweventreaction_t reaction_from_string(const std::string& str);

	struct FlowEventReactionData
	{
		floweventreaction_t reactions[PlayerType::SIZE] = { 0 };

		constexpr FlowEventReactionData(floweventreaction_t self, floweventreaction_t friends_and_strangers)
			: FlowEventReactionData(self, friends_and_strangers, friends_and_strangers)
		{
		}

		constexpr FlowEventReactionData(floweventreaction_t self, floweventreaction_t friends, floweventreaction_t strangers)
			: reactions{ self, friends, strangers }
		{
		}

		constexpr FlowEventReactionData(floweventreaction_t reaction)
			: FlowEventReactionData(reaction, reaction, reaction)
		{
		}

		constexpr FlowEventReactionData() = default;

		void setReactions(floweventreaction_t updated_value) noexcept
		{
			for (auto& r : reactions)
			{
				r = updated_value;
			}
		}

		floweventreaction_t& getReactions(playertype_t player_type = PlayerType::SELF);
		const floweventreaction_t& getReactions(playertype_t player_type = PlayerType::SELF) const;

		[[nodiscard]] std::string toString() const;
		[[nodiscard]] static FlowEventReactionData fromString(const std::string& str);
	};
	static_assert(sizeof(FlowEventReactionData) == sizeof(floweventreaction_t) * PlayerType::SIZE);
}
