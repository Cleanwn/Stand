#pragma once

#include "PlayerProvider.hpp"

#include "AbstractPlayer.hpp"

namespace Stand
{
	class PlayerProviderAll : public PlayerProvider
	{
	public:
		explicit PlayerProviderAll()
			: PlayerProvider(false)
		{
		}

		std::string getCommandName() const final;

		std::vector<AbstractPlayer> getPlayers(const bool include_self) const final;

		using PlayerProvider::getPlayers;
	};
}
