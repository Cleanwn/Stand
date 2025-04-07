#pragma once

#include "AbstractPlayer.hpp"

namespace Stand
{
	struct ReactionExemption
	{
		enum Config : uint8_t
		{
			OFF = 0,
			ON,
			ON_UNLESS_FRIEND
		};

		uint8_t value = OFF;

		[[nodiscard]] bool isEnabled() const
		{
			return value != OFF;
		}

		[[nodiscard]] bool shouldReactAgainst(AbstractPlayer check_against) const
		{
			if (!isEnabled())
			{
				return false;
			}

			switch (value)
			{
			case ON_UNLESS_FRIEND:
				if (check_against.isFriend())
				{
					return false;
				}
				break;
			}

			return true;
		}
	};
}