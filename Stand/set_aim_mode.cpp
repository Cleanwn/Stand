#include "set_aim_mode.hpp"

#include "natives.hpp"

namespace Stand
{
	void set_aim_mode(int aim_mode)
	{
		PLAYER::SET_PLAYER_TARGETING_MODE(aim_mode);
	}
}
