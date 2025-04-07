#include "FlowEventReaction.hpp"

#include <soup/string.hpp>

#include "FlowEvent.hpp"
#include "Gui.hpp"
#include "is_session.hpp"
#include "Util.hpp"

namespace Stand
{
	[[nodiscard]] static bool is_reaction_aggressive(floweventreaction_t reaction)
	{
		switch (reaction)
		{
		case REACTION_KICK:
		case REACTION_LL_KICK:
		case REACTION_BLACKLIST_KICK:
			return true;
		}
		return false;
	}

	[[nodiscard]] static bool is_reaction_ultimate_exclusive(flowevent_t event, floweventreaction_t reaction)
	{
		switch (reaction)
		{
		case REACTION_BLOCK:
			return FlowEvent::is_modder_detection(event);

		case REACTION_CRASH:
			return true;
		}
		if (is_reaction_aggressive(reaction))
		{
			return event == FlowEvent::MISC_JOIN;
		}
		return false;
	}

	commandflags_t get_reaction_flags(flowevent_t event, floweventreaction_t reaction)
	{
		commandflags_t flags = 0;
		if (is_reaction_aggressive(reaction))
		{
			flags |= CMDFLAG_FEATURELIST_BASIC;
		}
		if (is_reaction_ultimate_exclusive(event, reaction))
		{
			flags |= CMDFLAG_FEATURELIST_ULTIMATE;
		}
		return flags;
	}

	toast_t flow_event_reactions_to_logger_toast_flags(floweventreaction_t& reactions)
	{
		toast_t toast_flags = 0;
		if (reactions & REACTION_LOG_FILE)
		{
			toast_flags |= TOAST_FILE;
			reactions &= ~REACTION_LOG_FILE;
		}
		if (reactions & REACTION_LOG_CONSOLE)
		{
			toast_flags |= TOAST_CONSOLE;
			reactions &= ~REACTION_LOG_CONSOLE;
		}
		return toast_flags;
	}

	toast_t flow_event_reactions_to_toast_flags(floweventreaction_t& reactions)
	{
		toast_t toast_flags = flow_event_reactions_to_logger_toast_flags(reactions);
		if (reactions & REACTION_TOAST)
		{
			toast_flags |= TOAST_DEFAULT;
			reactions &= ~REACTION_TOAST;
		}
		if (reactions & REACTION_CHAT)
		{
			if (g_gui.players_discovered || is_session_transition_active(true))
			{
				toast_flags |= TOAST_CHAT;
			}
			reactions &= ~(REACTION_CHAT | REACTION_CHAT_TEAM);
		}
		else if (reactions & REACTION_CHAT_TEAM)
		{
			if (g_gui.players_discovered || is_session_transition_active(true))
			{
				toast_flags |= TOAST_CHAT_TEAM;
			}
			reactions &= ~REACTION_CHAT_TEAM;
		}
		return toast_flags;
	}

	floweventreaction_t toast_flags_to_flow_event_reactions(toast_t tf)
	{
		floweventreaction_t reactions = 0;
		if ((tf & TOAST_DEFAULT) != 0)
		{
			reactions |= REACTION_TOAST;
		}
		if (tf & TOAST_FILE)
		{
			reactions |= REACTION_LOG_FILE;
		}
		if (tf & TOAST_CONSOLE)
		{
			reactions |= REACTION_LOG_CONSOLE;
		}
		if (tf & TOAST_CHAT)
		{
			reactions |= REACTION_CHAT;
		}
		else if (tf & TOAST_CHAT_TEAM)
		{
			reactions |= REACTION_CHAT_TEAM;
		}
		return reactions;
	}

	bool is_reaction_a_bad_idea(floweventreaction_t reaction, bool broad_event)
	{
		if (broad_event)
		{
			switch (reaction)
			{
			case REACTION_LOG_FILE:
			case REACTION_LOG_CONSOLE:
				return false;
			}
			return true;
		}

		switch (reaction)
		{
		case REACTION_NET_TOAST:
		case REACTION_SYNCIN_TOAST:
		case REACTION_SYNCOUT_TOAST:
			return true;
		}
		return false;
	}

	std::string reaction_to_string(floweventreaction_t reaction)
	{
		return Util::to_hex_string(reaction);
	}

	floweventreaction_t reaction_from_string(const std::string& str)
	{
		if (!str.empty())
		{
			return std::stoul(str, nullptr, 16);
		}
		return {};
	}

	floweventreaction_t& FlowEventReactionData::getReactions(playertype_t player_type)
	{
		return reactions[player_type];
	}

	const floweventreaction_t& FlowEventReactionData::getReactions(playertype_t player_type) const
	{
		return reactions[player_type];
	}

	std::string FlowEventReactionData::toString() const
	{
		std::string str{};
		for (playertype_t i = PlayerType::SIZE - 1;; --i)
		{
			str.append(reaction_to_string(getReactions(i)));
			if (i == 0)
			{
				break;
			}
			bool following_are_zero = true;
			for (playertype_t j = i;;)
			{
				if (getReactions(--j) != 0)
				{
					following_are_zero = false;
					break;
				}
				if (j == 0)
				{
					break;
				}
			}
			if (following_are_zero)
			{
				break;
			}
			str.append(1, ',');
		}
		return str;
	}

	FlowEventReactionData FlowEventReactionData::fromString(const std::string& str)
	{
		FlowEventReactionData data{};
		auto chunks = soup::string::explode(str, ',');
		playertype_t i = 0;
		for (playertype_t p = PlayerType::SIZE - 1; chunks.size() > i; ++i, --p)
		{
			data.reactions[p] = reaction_from_string(chunks.at(i));
		}
		return data;
	}
}
