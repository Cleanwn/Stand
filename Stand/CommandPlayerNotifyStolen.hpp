#pragma once

#include "CommandPlayerNotify.hpp"

#include "script_events.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerNotifyStolen : public CommandPlayerNotify
	{
	public:
		explicit CommandPlayerNotifyStolen(CommandList* const parent)
			: CommandPlayerNotify(parent, Util::i32_to_u64_trickery(SE_NOTIFY_STOL), LOC("PLY_NOTIFY_STOL"), { CMDNAME("notifystolen") })
		{
		}
	};
}
