#pragma once

#include "netPeerId.hpp"
#include "netSocketAddress.hpp"

namespace Stand
{
	class IceQueen
	{
	public:
		struct Peer
		{
			rage::netSocketAddress lan_addr{};
			rage::netSocketAddress public_addr{};

			rage::netPeerId startup_id;

			Peer(rage::netPeerId startup_id);
		};

		[[nodiscard]] static Peer& findOrCreatePeer(rage::netPeerId startup_id);
		[[nodiscard]] static Peer* findPeer(rage::netPeerId startup_id);
	private:
		[[nodiscard]] static Peer& createPeer(rage::netPeerId startup_id);
		[[nodiscard]] static std::vector<Peer>& getPeerList();
	};
}
