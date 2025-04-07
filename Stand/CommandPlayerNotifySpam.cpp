#include "CommandPlayerNotifySpam.hpp"

#include "AbstractPlayer.hpp"
#include "script_events.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandPlayerNotifySpam::CommandPlayerNotifySpam(CommandList* const parent)
		: CommandPlayerToggleBatch(parent, LOC("PLY_NOTIFY_SPAM"), CMDNAMES("notifyspam"), NOLABEL, COMMANDPERM_RUDE)
	{
	}

	void CommandPlayerNotifySpam::onTick(const std::vector<AbstractPlayer>& players) const
	{
		static uint64_t value = 0;
		static uint8_t notify;

		uint64_t notify_hash;
		switch (notify)
		{
		case 0:
			notify_hash = Util::i32_to_u64_trickery(SE_NOTIFY_REMO);
			notify++;
			break;

		case 1:
			notify_hash = Util::i32_to_u64_trickery(SE_NOTIFY_STOL);
			notify++;
			break;

		case 2:
			notify_hash = Util::i32_to_u64_trickery(SE_NOTIFY_BANK);
			notify = 0;
			break;
		}

		for (const AbstractPlayer p : players)
		{
			uint64_t args[] = {
				Util::i32_to_u64_trickery(SE_NOTIFY),
				g_player,
				notify_hash,
				value
			};
			TSE(1 << p, args);
		}

		value++;
	}
}
