#pragma once

#include <string>

#include "fwddecl.hpp"
#include "typedecl.hpp"

namespace Stand
{
#define HP_TRACK					0b1
#define HP_COPY						0b10
#define HP_SCNAME					0b100
#define HP_WHITELIST				0b100000
#define HP_FAKE_FRIEND				0b1000000

#define HP_VALID_FLAGS_V9 (HP_TRACK | HP_COPY | HP_SCNAME | HP_WHITELIST | HP_FAKE_FRIEND)

#pragma pack(push, 1)
	class HistoricPlayer
	{
	public:
		int64_t account_id;
		int64_t rid;
		uint32_t mac_id;
		std::string name;
		unixtime_t first_seen;
		unixtime_t last_seen;
		floweventreaction_t join_reactions;
		std::string note;
		uint8_t flags;

		explicit HistoricPlayer(int64_t account_id, int64_t rid, uint32_t mac_id, std::string&& name, unixtime_t first_seen, unixtime_t last_seen, floweventreaction_t join_reactions, std::string&& note, uint8_t flags);

		void onImportedFromFormatVersion9OrBelow() noexcept
		{
			flags &= HP_VALID_FLAGS_V9;
		}

		void processFlags()
		{
			if (flags & (HP_TRACK | HP_COPY))
			{
				processFlagsImpl();
			}
		}

		void preRemove()
		{
			if (isTracked())
			{
				untrack();
			}
		}

	private:
		void processFlagsImpl();

	public:
		void setGamerId(uint64_t gamer_id) noexcept;
		void setLastSeenNow() noexcept;

		void assimilate(HistoricPlayer& b);

		[[nodiscard]] AbstractPlayer inSession() const;
		[[nodiscard]] bool isFriend() const;

		[[nodiscard]] ScAccount* getScAccount() const;
		[[nodiscard]] std::string getNameForUser() const;
		
		struct Tracking
		{
			uint8_t value;

			[[nodiscard]] bool isEnabled() const
			{
				return value != 0;
			}

			[[nodiscard]] bool isFakeFriend() const
			{
				return value == 2;
			}

			bool operator ==(const Tracking& b) const noexcept
			{
				return value == b.value;
			}

			bool operator !=(const Tracking& b) const noexcept
			{
				return !operator==(b);
			}
		};

		[[nodiscard]] bool isTracked() const noexcept { return (flags & HP_TRACK); }
		[[nodiscard]] bool isFakeFriend() const noexcept { return (flags & HP_FAKE_FRIEND); }
		[[nodiscard]] Tracking getTracking() const;
		bool setTracking(Tracking tracking);
		void track();
		void untrack();
	};
#pragma pack(pop)
}
