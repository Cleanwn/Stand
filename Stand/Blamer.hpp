#pragma once

#include "typedecl.hpp"
#include "gta_player.hpp"

#include "get_current_time_millis.hpp"

namespace Stand
{
	class Blamer
	{
	private:
		const time_t deadline;
		time_t when = 0;
		compactplayer_t who;

	public:
		Blamer(time_t deadline)
			: deadline(deadline)
		{
		}

		void setBlame(compactplayer_t p) noexcept
		{
			when = get_current_time_millis();
			who = p;
		}

		[[nodiscard]] compactplayer_t getBlame() const noexcept
		{
			if (GET_MILLIS_SINCE(when) < deadline)
			{
				return who;
			}
			return MAX_PLAYERS;
		}
	};
}
