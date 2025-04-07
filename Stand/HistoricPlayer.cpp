#include "HistoricPlayer.hpp"

#include <fmt/format.h>

#include "AbstractPlayer.hpp"
#include "CommandPlayer.hpp"
#include "get_seconds_since_unix_epoch.hpp"
#include "natives.hpp"
#include "PlayerHistory.hpp"
#include "RemoteGamer.hpp"
#include "rlGamerId.hpp"
#include "ScAccount.hpp"
#include "SessionSpoofing.hpp"

namespace Stand
{
	HistoricPlayer::HistoricPlayer(int64_t account_id, int64_t rid, uint32_t mac_id, std::string&& name, time_t first_seen, time_t last_seen, floweventreaction_t join_reactions, std::string&& note, uint8_t flags)
		: account_id(account_id), rid(rid), mac_id(mac_id), name(std::move(name)), first_seen(first_seen), last_seen(last_seen), join_reactions(join_reactions), note(std::move(note)), flags(flags)
	{
	}

	void HistoricPlayer::processFlagsImpl()
	{
		auto* g = RemoteGamer::get(rid, true, RGU_NONE);
		if (g != nullptr)
		{
			if (isTracked())
			{
				g->track();
				g->fake_friend = isFakeFriend();
			}
			if (flags & HP_COPY)
			{
				SessionSpoofing::copy_from = g;
			}
		}
	}

	void HistoricPlayer::setLastSeenNow() noexcept
	{
		const auto unixtime = get_seconds_since_unix_epoch();
		if (first_seen == 0)
		{
			first_seen = unixtime;
		}
		last_seen = unixtime;
	}

	void HistoricPlayer::assimilate(HistoricPlayer& b)
	{
		join_reactions |= b.join_reactions;

		flags |= (b.flags & (HP_TRACK | HP_COPY));

		/*if ((flags & HP_GAMERIDCOULDBESPOOFED)
			&& !(b.flags & HP_GAMERIDCOULDBESPOOFED)
			)
		{
			scrambled_gamer_id = b.scrambled_gamer_id;
			flags &= ~HP_GAMERIDCOULDBESPOOFED;
		}*/

		if (name != b.name)
		{
			if (b.flags & HP_SCNAME)
			{
				name = std::move(b.name);
				flags |= HP_SCNAME;
			}
			else
			{
				//if (getGamerId().isCongruentWithName(b.name))
				{
					name = std::move(b.name);
					flags &= ~HP_SCNAME;
				}
			}
		}
		else
		{
			if (b.flags & HP_SCNAME)
			{
				flags |= HP_SCNAME;
			}
		}
	}

	AbstractPlayer HistoricPlayer::inSession() const
	{
		for (const auto& p : AbstractPlayer::listAll())
		{
			if (auto cmd = p.getCommand())
			{
				if (cmd->historic_player == this)
				{
					return p;
				}
			}
		}
		return AbstractPlayer::invalid();
	}

	bool HistoricPlayer::isFriend() const
	{
		Any network_handle[13 * 2];
		auto rid_str = fmt::to_string(rid);
		NETWORK::NETWORK_HANDLE_FROM_MEMBER_ID(rid_str.c_str(), network_handle, 13);
		return NETWORK::NETWORK_IS_FRIEND(network_handle);
	}

	ScAccount* HistoricPlayer::getScAccount() const
	{
		return ScAccount::fromRID(rid);
	}

	std::string HistoricPlayer::getNameForUser() const
	{
		return ScAccount::formatName(rid, name);
	}

	using Tracking = HistoricPlayer::Tracking;

	Tracking HistoricPlayer::getTracking() const
	{
		// Get bit 1
		if (!(flags & HP_TRACK))
		{
			return Tracking{ 0 };
		}
		// Get bit 2
		if (!(flags & HP_FAKE_FRIEND))
		{
			return Tracking{ 1 };
		}
		return Tracking{ 2 };
	}

	bool HistoricPlayer::setTracking(Tracking tracking)
	{
		const auto prev = getTracking();

		if (tracking != prev)
		{
			if (tracking.isEnabled())
			{
				if (!prev.isEnabled())
				{
					// Begin tracking
					auto* g = RemoteGamer::get(rid, true, RGU_NONE);
					if (g == nullptr)
					{
						return false;
					}
					g->track();
					g->fake_friend = tracking.isFakeFriend();
				}
				else
				{
					auto* g = RemoteGamer::getSimple(rid);
					if (g != nullptr)
					{
						g->fake_friend = tracking.isFakeFriend();
					}
				}
			}
			else
			{
				if (prev.isEnabled())
				{
					// End tracking
					auto* g = RemoteGamer::getSimple(rid);
					if (g != nullptr)
					{
						g->untrack();
					}
				}
			}

			// Set bit 1
			if (tracking.isEnabled())
			{
				flags |= HP_TRACK;
			}
			else
			{
				flags &= ~HP_TRACK;
			}

			// Set bit 2
			if (tracking.isFakeFriend())
			{
				flags |= HP_FAKE_FRIEND;
			}
			else
			{
				flags &= ~HP_FAKE_FRIEND;
			}
		}

		return true;
	}

	void HistoricPlayer::track()
	{
		if (!isTracked())
		{
			setTracking(Tracking{ 1 });
		}
	}

	void HistoricPlayer::untrack()
	{
		setTracking(Tracking{ 0 });
	}
}
