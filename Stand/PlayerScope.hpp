#pragma once

#include <cstdint>

namespace Stand
{
	using playerscope_t = uint8_t;

	struct PlayerScope
	{
		enum _ : playerscope_t
		{
			NOONE,
			SELF,
			FRIENDS,
			CREW_MEMBERS,
			EVERYONE
		};
	};
}
