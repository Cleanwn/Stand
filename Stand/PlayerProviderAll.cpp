#include "PlayerProviderAll.hpp"

#include "CommandToggleNoCorrelation.hpp"

namespace Stand
{
	std::string PlayerProviderAll::getCommandName() const
	{
		return "all";
	}

	std::vector<AbstractPlayer> PlayerProviderAll::getPlayers(const bool include_self) const
	{
		return excludes.getNonExcludedPlayers(!include_self);
	}
}
