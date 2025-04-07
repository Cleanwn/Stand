#include "AddressGamers.hpp"

#include <fmt/core.h>

#include "fmt_arg.hpp"
#include "lang.hpp"

namespace Stand
{
	using Gamer = AddressGamers::Gamer;
	using Peer = AddressGamers::Peer;

	void Gamer::reset(const std::string& name, int64_t rid_2)
	{
		this->name = name;
		this->rid_2 = rid_2;
	}

	Peer* AddressGamers::findPeer(rage::netIpAddress ip)
	{
		for (auto& peer : peers)
		{
			if (peer.ip == ip)
			{
				return &peer;
			}
		}
		return nullptr;
	}

	Peer* AddressGamers::findOrCreatePeer(rage::netIpAddress ip)
	{
		auto peer = findPeer(ip);
		if (peer == nullptr)
		{
			peer = &peers.emplace_back(ip);
		}
		return peer;
	}

	Gamer* AddressGamers::findGamerByRid2(int64_t rid_2)
	{
		for (auto& peer : peers)
		{
			if (peer.gamer.rid_2 == rid_2)
			{
				return &peer.gamer;
			}
		}
		return nullptr;
	}

	void AddressGamers::onDiscoveredName(rage::netIpAddress ip, const char* name)
	{
		findOrCreatePeer(ip)->gamer.name = name;
	}

	void AddressGamers::onDiscoveredGamer(rage::netIpAddress ip, const rage::rlGamerInfo& gamerInfo)
	{
		findOrCreatePeer(ip)->gamer.reset(gamerInfo.name, gamerInfo.getHandle().rockstar_id);
	}

	std::string AddressGamers::getNameForUser(const rage::netIpAddress& ip)
	{
		return getNameForUser(findPeer(ip), ip);
	}

	std::string AddressGamers::getNameForUser(const Peer* peer, const rage::netIpAddress& ip)
	{
		if (peer && !peer->gamer.name.empty())
		{
			return LANG_FMT("IPNAM", FMT_ARG("ip", ip.toString<std::string>()), FMT_ARG("name", peer->gamer.name.c_str()));
		}
		return ip.toString<std::string>();
	}

	void AddressGamers::deinit()
	{
		peers.clear();
	}
}
