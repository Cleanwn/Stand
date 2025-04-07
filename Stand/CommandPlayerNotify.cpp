#include "CommandPlayerNotify.hpp"

#include "AbstractPlayer.hpp"
#include "script_events.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandPlayerNotify::CommandPlayerNotify(CommandList* const parent, uint64_t notify_hash, Label&& menu_name, std::vector<CommandName>&& command_names)
		: CommandPlayerActionValue(parent, INT_MIN, INT_MAX, std::move(menu_name), std::move(command_names), NOLABEL, COMMANDPERM_RUDE), notify_hash(notify_hash)
	{
	}

	void CommandPlayerNotify::onCommandWithValue(Click& click, long long value)
	{
		click.ensureScriptThread([this, value]
		{
			DEF_P2;
			for (const AbstractPlayer p : pp->getPlayers(pp->single))
			{
				uint64_t args[] = {
					Util::i32_to_u64_trickery(SE_NOTIFY),
					g_player,
					notify_hash,
					(uint64_t)value,
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					0,
					0
				};
				TSE(1 << p, args);
			}
		});
	}
}
