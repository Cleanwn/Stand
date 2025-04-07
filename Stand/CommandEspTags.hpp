#pragma once

#include "CommandList.hpp"

#include "CommandEspTagsItem.hpp"

namespace Stand
{
	class CommandEspTags : public CommandList
	{
	private:
		CommandColour* const default_colour = nullptr;

		static inline playerflag_t flags[] = {
			FLAG_FRIEND,
			FLAG_ORG_MEMBER,
			FLAG_MOD,
			FLAG_LIKELY_MODDER,
			FLAG_GOD,
			FLAG_VEHGOD,
			FLAG_OTR,
			FLAG_INVISIBLE,
			FLAG_CREW,
			FLAG_DEAD,
			FLAG_RC,
			FLAG_TYPING,
			FLAG_SPEC,
			FLAG_INT,
			FLAG_ATTACKED,
			FLAG_VPN,
			FLAG_HOST,
			FLAG_NEXT_HOST,
			FLAG_SCRIPT_HOST

			// Excluded: FLAG_SELF, FLAG_TIMEOUT, FLAG_GEOIP
		};

		std::vector<CommandEspTagsItem*> colours;

	public:
		explicit CommandEspTags(CommandList* const parent, CommandColour* const default_colour)
			: CommandList(parent, LOC("TGCLRS"), {}, LOC("ESPTAGS_H")), default_colour(default_colour)
		{
			for (size_t i = 0; i < COUNT(flags); ++i)
			{
				colours.emplace_back(createChild<CommandEspTagsItem>(flags[i]));
			}
		}

		[[nodiscard]] DirectX::SimpleMath::Color getColour(AbstractPlayer& p) const
		{
			for (size_t i = 0; i < COUNT(flags); ++i)
			{
				if (p.hasFlag(flags[i]))
				{
					if (auto cmd = colours.at(i); cmd->isEnabled())
					{
						return cmd->getColour();
					}
				}
			}

			return default_colour->getRGBA();
		}
	};
}