#pragma once

#include "PlayerProvider.hpp"

namespace Stand
{
	class PlayerProviderSingle : public PlayerProvider
	{
	public:
		const compactplayer_t p;

		explicit PlayerProviderSingle(const compactplayer_t p)
			: PlayerProvider(true), p(p)
		{
		}

		std::string getCommandName() const final
		{
			std::string name = AbstractPlayer(p).getName();
			StringUtils::simplifyCommandName(name);
			return name;
		}

		std::vector<AbstractPlayer> getPlayers(const bool include_self) const final
		{
			if (include_self || p != g_player)
			{
				return std::vector<AbstractPlayer>(1, AbstractPlayer(p));
			}
			return std::vector<AbstractPlayer>();
		}

		[[nodiscard]] AbstractPlayer getPlayer() const
		{
			return p;
		}
	};
}
