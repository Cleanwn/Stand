#include "rlGamerInfo.hpp"

#include <fmt/format.h>

#include "AbstractPlayer.hpp"
#include "Blacklist.hpp"
#include "cidr.hpp"
#include "CNetworkSession.hpp"
#include "Exceptional.hpp"
#include "FlowEvent.hpp"
#include "Hooking.hpp"
#include "is_rid.hpp"
#include "is_session.hpp"
#include "NuancedEvent.hpp"
#include "pointers.hpp"
#include "rockstar_admins.hpp"
#include "ScAccount.hpp"

namespace rage
{
	using namespace Stand;

	std::string rlGamerInfo::getNameForUser() const
	{
		if (AbstractPlayer::streamer_spoof != 0)
		{
			return AbstractPlayer::getMaskName(name);
		}
		if (*name == '\0')
		{
			return ScAccount::fromRID(getHandle().rockstar_id)->toString(false);
		}
		return name;
	}

	std::string rlGamerInfo::toString() const
	{
		std::string str = getNameForUser();

		std::string extra;
		if (AbstractPlayer::hide_rid <= PlayerScope::SELF)
		{
			extra.append(fmt::to_string(peer.address.gamer_handle.rockstar_id));
		}
		if (AbstractPlayer::hide_ip <= PlayerScope::SELF)
		{
			auto addr = peer.address.getAddress();
			if (addr.ip != -1)
			{
				if (!extra.empty())
				{
					extra.append(" / ");
				}
				extra.append(addr.ip.operator std::string());
			}
		}
		if (!extra.empty())
		{
			str.append(" (");
			str.append(extra);
			str.push_back(')');
		}

		return str;
	}

	std::vector<Stand::NuancedEvent> rlGamerInfo::checkDetections(bool is_self)
	{
		std::vector<Stand::NuancedEvent> res{};

		__try
		{
			soup::native_u32_t ip_address;

			if (is_self)
			{
				ip_address = peer.address.local_only_online.ip.value;
			}
			else
			{
				ip_address = peer.address.getPublicAddress().ip.value;
			}

			//Util::toast(fmt::format("{} -> {}", name, soup::IpAddr(ip_address).toString()));

			if (CidrSupernets::take_two.contains(ip_address))
			{
				res.emplace_back(FlowEvent::MOD_T2IP);
			}
		}
		__EXCEPTIONAL()
		{
		}

		if (is_admin_rid(peer.address.gamer_handle.rockstar_id))
		{
			res.emplace_back(FlowEvent::MOD_ADMINRID);
		}

		if (!is_self)
		{
			res.emplace_back(FlowEvent::MOD_PRESENCESPOOFING, 0);
		}

		// Possible false-positive:
		// - 207002828 has a really fortunate mac address (DF-0E-FA-14-B9-8D), hashing to 0x001C9204.
		// - The startup id matches so either it's their actual mac address or it was spoofed really well.
		// - I also can't find that mac address in any database, so that makes it being real less likely.
		if (peer.id <= 10000000000000000i64)
		{
			if (peer.id < 9000000000000000i64)
			{
				res.emplace_back(FlowEvent::MOD_HOSTKN_AGGRESSIVE);
			}
			else
			{
				res.emplace_back(FlowEvent::MOD_HOSTKN_SWEET);
			}
		}
		else if (peer.id >= (0xFFFFFFFFFFFFFFFF - 0xFFFF))
		{
			res.emplace_back(FlowEvent::MOD_HOSTKN_HANDICAP);
		}
		else if (peer.id < peer.address.startup_id
			|| (peer.id - peer.address.startup_id) > 0x20000
			)
		{
			res.emplace_back(FlowEvent::MOD_SPFDHOSTKN);
		}

		return res;
	}

	bool rlGamerInfo::isHost() const
	{
		return (*pointers::network_session)->session->host_peer_id == peer.id;
	}

	bool rlGamerInfo::isUser() const
	{
		return g_player.getRockstarId() == getHandle().rockstar_id;
	}

	bool rlGamerInfo::isKnownAdvertiser() const
	{
		if (Blacklist::isAdvertiserV2(*this))
		{
			return true;
		}
		std::wstring wname = StringUtils::utf8_to_utf16(name);
		AbstractPlayer::normaliseForAdFilters(wname);
		return AbstractPlayer::checkKnownAggressiveSpammer(wname);
	}
}
