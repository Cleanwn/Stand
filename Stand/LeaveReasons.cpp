#include "LeaveReasons.hpp"

#include <fmt/core.h>

#include "AbstractPlayer.hpp"
#include "CNetworkSession.hpp"
#include "eBailReason.hpp"
#include "fmt_arg.hpp"
#include "get_current_time_millis.hpp"
#include "is_session.hpp"
#include "pointers.hpp"
#include "rlGamerInfo.hpp"
#include "ScriptGlobal.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"

namespace Stand
{
	void LeaveReasons::onRemovedGamer(const rage::rlGamerInfo& gamer, int remove_reason)
	{
		// We don't care if gamer claims to have been banned
		if (remove_reason == BAIL_CONSOLE_BAN)
		{
			remove_reason = BAIL_INVALID;
		}

		PeerData& data = getEntry(gamer);
		if (!data.no_removed_gamer_notify)
		{
			if (data.left_freemode != 0 && GET_MILLIS_SINCE(data.left_freemode) > 20000)
			{
				// freemode death kick: removed from session 30 seconds after leaving freemode; gamer does not broadcast snMsgRemoveGamersFromSessionCmd.
				conclude(gamer, LOC("LR_FMD"));
			}
			else if (data.breakup_from < MAX_PLAYERS)
			{
				conclude(gamer, LOC("LR_K_B"), data.breakup_from);
			}
			else if (data.physical < MAX_PLAYERS && ScriptGlobal(GLOBAL_VOTE_KICKED).at(data.physical, 1).get<int>() == 1)
			{
				if (data.vote_kicked)
				{
					conclude(gamer, LOC("LR_VK"));
				}
				else
				{
					conclude(gamer, LOC("LR_SHK"), AbstractPlayer::script_host);
				}
			}
			else if ((*pointers::network_session)->blacklist.IsBlacklisted(gamer.getHandle()))
			{
				conclude(gamer, LOC("LR_HK"), AbstractPlayer::host);
			}
			else if (data.was_timing_out)
			{
				conclude(gamer, LOC("LR_T"));
			}
			else if (data.love_letter_from < MAX_PLAYERS)
			{
				if (data.love_letter_to == data.love_letter_from)
				{
					conclude(gamer, LOC("LR_DL"), data.love_letter_from);
				}
				else
				{
					conclude(gamer, LOC("LR_LL"), data.love_letter_from);
				}
			}
			else if (remove_reason >= 0)
			{
				if (remove_reason == BAIL_EXIT_GAME)
				{
					if (closed_game_enabled)
					{
						conclude(gamer, LOC("LR_Q"));
					}
				}
				else if (remove_reason == BAIL_FROM_SCRIPT)
				{
					concludeDueTo(gamer, soup::ObfusString("NETWORK_BAIL"));
				}
				else if (remove_reason == BAIL_START_TIMED_OUT)
				{
					if (blocked_from_joining_enabled)
					{
						conclude(gamer, LOC("LR_B"));
					}
				}
				else if (remove_reason == BAIL_OUT_OF_MEMORY)
				{
					conclude(gamer, LOC("LR_K_O"));
				}
				else if (remove_reason == BAIL_PROFILE_CHANGE) // SHUTDOWN_AND_LOAD_MOST_RECENT_SAVE
				{
					conclude(gamer, LOC("LR_MOD"));
				}
				else if (remove_reason == BAIL_INVALIDATED_ROS_TICKET)
				{
					conclude(gamer, LOC("LR_BAN"));
				}
				else
				{
					concludeUnknown(gamer, remove_reason);
				}
			}
			else
			{
				if (AbstractPlayer::fromPeerId(gamer.peer.id).isValid())
				{
					if (normal_enabled)
					{
						conclude(gamer, LOC("LR_N"));
					}
				}
				else if (!is_session_transition_active(true))
				{
					if (data.left_freemode == 0)
					{
						if (blocked_from_joining_enabled)
						{
							conclude(gamer, LOC("LR_B"));
						}
					}
					else
					{
						concludeUnknown(gamer, -1);
					}
				}
			}
		}
		forget(gamer);
	}

	void LeaveReasons::onTimedOut(const rage::rlGamerInfo& gamer)
	{
		PeerData& data = getEntry(gamer);
		data.no_removed_gamer_notify = true;
		if (data.left_freemode == 0)
		{
			conclude(gamer, LOC("LR_T"));
		}
		else
		{
			conclude(gamer, LOC("LR_FMDC"));
		}
	}

	PeerData& LeaveReasons::getEntry(const rage::rlGamerInfo& gamer)
	{
		return getEntry(gamer.peer.id);
	}

	PeerData& LeaveReasons::getEntry(rage::netPeerId peerId)
	{
		auto e = peers.find(peerId);
		if (e != peers.end())
		{
			return e->second;
		}
		return peers.emplace(peerId, PeerData{}).first->second;
	}

	void LeaveReasons::conclude(const rage::rlGamerInfo& gamer, const Label& reason)
	{
		if (auto tf = toast_config.getToastFlags())
		{
			Util::toast(fmt::format(fmt::runtime(reason.getLocalisedUtf8()), FMT_ARG("player", gamer.getNameForUser())), tf);
		}
	}

	void LeaveReasons::conclude(const rage::rlGamerInfo& gamer, const Label& reason, const compactplayer_t other)
	{
		if (auto tf = toast_config.getToastFlags())
		{
			Util::toast(fmt::format(fmt::runtime(reason.getLocalisedUtf8()), FMT_ARG("player", gamer.getNameForUser()), FMT_ARG("other", AbstractPlayer(other).getName())), tf);
		}
	}

	void LeaveReasons::concludeDueTo(const rage::rlGamerInfo& gamer, const std::string& reason)
	{
		if (auto tf = toast_config.getToastFlags())
		{
			Util::toast(LANG_FMT("LR_E", FMT_ARG("player", gamer.getNameForUser()), FMT_ARG("reason", reason)), tf);
		}
	}

	void LeaveReasons::concludeUnknown(const rage::rlGamerInfo& gamer, const int id)
	{
		if (auto tf = toast_config.getToastFlags())
		{
			Util::toast(LANG_FMT("LR_U", FMT_ARG("player", gamer.getNameForUser()), FMT_ARG("id", id)), tf);
		}
	}

	void LeaveReasons::forget(const rage::rlGamerInfo& gamer)
	{
		peers.erase(gamer.peer.id);
	}
}
