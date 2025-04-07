#pragma once

#include <string>

#include <soup/Packet.hpp>

namespace Stand
{
	SOUP_PACKET(HistoricPlayerPacketV6)
	{
		u64 rid;
		std::string name;
		u64 first_seen;
		u64 last_seen;
		std::string note;
		u8 flags;
		u64 scrambled_gamer_id;
		u64 join_reactions;

		SOUP_PACKET_IO(s)
		{
			return s.u64_dyn(rid)
				&& s.str_nt(name)
				&& s.u64_dyn(first_seen)
				&& s.u64_dyn(last_seen)
				&& s.str_nt(note)
				&& s.u8(flags)
				&& s.u64(scrambled_gamer_id)
				&& s.u64_dyn(join_reactions)
				;
		}
	};

	SOUP_PACKET(HistoricPlayerPacketV7)
	{
		HistoricPlayerPacketV6 v6;
		u32 mac_id;

		SOUP_PACKET_IO(s)
		{
			return v6.io(s)
				&& s.u32(mac_id)
				;
		}
	};

	SOUP_PACKET(HistoricPlayerPacketV8)
	{
		HistoricPlayerPacketV7 v7;
		u64 account_id;

		SOUP_PACKET_IO(s)
		{
			return v7.io(s)
				&& s.u64(account_id)
				;
		}
	};

	SOUP_PACKET(HistoricPlayerPacketV10)
	{
		u64 rid;
		std::string name;
		u64 first_seen;
		u64 last_seen;
		std::string note;
		u8 flags;
		u64 join_reactions;
		u32 mac_id;
		u64 account_id;

		SOUP_PACKET_IO(s)
		{
			return s.u64_dyn(rid)
				&& s.str_nt(name)
				&& s.u64_dyn(first_seen)
				&& s.u64_dyn(last_seen)
				&& s.str_nt(note)
				&& s.u8(flags)
				&& s.u64_dyn(join_reactions)
				&& s.u32(mac_id)
				&& s.u64_dyn(account_id)
				;
		}
	};

	template <typename HistoricPlayerPacket>
	struct PlayerHistoryPacket : public ::soup::Packet<PlayerHistoryPacket<HistoricPlayerPacket>>
	{
		std::vector<HistoricPlayerPacket> players{};

		SOUP_PACKET_IO(s)
		{
			if (s.isRead())
			{
				uint64_t size;
				if (!s.u64_dyn(size))
				{
					return false;
				}
				for (; size != 0; --size)
				{
					HistoricPlayerPacket p;
					if (!p.io(s))
					{
						return false;
					}
					players.emplace_back(p);
				}
			}
			else if (s.isWrite())
			{
				uint64_t size = players.size();
				if (!s.u64_dyn(size))
				{
					return false;
				}
				for (auto& p : players)
				{
					if (!p.io(s))
					{
						return false;
					}
				}
			}
			return true;
		}
	};
}
