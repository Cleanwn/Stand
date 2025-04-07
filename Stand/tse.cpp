#include "tse.hpp"

#include "AbstractPlayer.hpp"
#include "natives.hpp"
#include "pointers.hpp"

namespace Stand
{
	void tse_impl(void* args, int args_count, unsigned int player_bitflags)
	{
		if (*pointers::is_session_started
			&& AbstractPlayer::canSendNetEvents()
			)
		{
			const auto event_hash = *(int64_t*)args;

			auto data = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(args) + (2 * sizeof(int64_t)));
			auto data_count = args_count - 2;

			args_count = data_count + 3;
			args = malloc(args_count * sizeof(int64_t));
			((int64_t*)args)[0] = event_hash;
			((int64_t*)args)[1] = (int64_t)(int)g_player;
			((int64_t*)args)[2] = (int64_t)(uint64_t)player_bitflags;
			if (data_count > 0)
			{
				memcpy(&((int*)args)[3 * 2], data, data_count * sizeof(int64_t));
			}
			SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, (Any*)args, args_count, player_bitflags, (Hash)event_hash);
			free(args);
		}
	}
}
