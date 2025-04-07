#include "IceQueen.hpp"

#include "CrossState.hpp"

namespace Stand
{
	using Peer = IceQueen::Peer;

	Peer::Peer(rage::netPeerId startup_id)
		: startup_id(startup_id)
	{
	}

	Peer& IceQueen::findOrCreatePeer(rage::netPeerId startup_id)
	{
		if (auto peer = findPeer(startup_id))
		{
			return *peer;
		}
		return createPeer(startup_id);
	}

	Peer* IceQueen::findPeer(rage::netPeerId startup_id)
	{
		for (auto& peer : getPeerList())
		{
			if (peer.startup_id == startup_id)
			{
				return &peer;
			}
		}
		return nullptr;
	}

	Peer& IceQueen::createPeer(rage::netPeerId startup_id)
	{
		std::vector<Peer>& peers = getPeerList();

		if (peers.size() == 200)
		{
			peers.erase(peers.begin());
		}

		return peers.emplace_back(Peer(startup_id));
	}

	struct IceQueenPeerListV2
	{
		std::vector<Peer> peers{};
	};

	std::vector<Peer>& IceQueen::getPeerList()
	{
		return CrossState::get().struct_map.getStructFromMap(IceQueenPeerListV2).peers;
	}
}
