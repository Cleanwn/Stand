#pragma once

#include "CommandPlayerNotify.hpp"

#include "script_events.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerNotifyBanked : public CommandPlayerNotify
	{
	public:
		explicit CommandPlayerNotifyBanked(CommandList* const parent)
			: CommandPlayerNotify(parent, Util::i32_to_u64_trickery(SE_NOTIFY_BANK), LOC("PLY_NOTIFY_BANK"), { CMDNAME("notifybanked") })
		{
		}
	};
}
