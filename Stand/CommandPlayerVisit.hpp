#pragma once

#include "CommandPlayerListPlayersShortcut.hpp"

namespace Stand
{
	class CommandPlayerVisit : public CommandPlayerListPlayersShortcut
	{
	public:
		explicit CommandPlayerVisit(CommandList* const parent)
			: CommandPlayerListPlayersShortcut(parent, LOC("PLY_VISIT"))
		{
		}

		Label getInvalidityReason(const PlayerProvider* blamed_on) const final
		{
			AbstractPlayer ap = blamed_on->getPlayers(true).at(0);
			if (ap.getApartmentId() < 1)
			{
				return LIT(LANG_FMT("PLYNAPT", ap.getName()));
			}
			return NOLABEL;
		}

		std::string getChild(AbstractPlayer& blamed_on, const std::string& target) const final
		{
			return fmt::format("as {} apt{} {}", blamed_on.getName(), blamed_on.getApartmentId(), target);
		}
	};
}
