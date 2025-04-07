#pragma once

#include "CommandPlayerAction.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "PlayerProvider.hpp"
#include "script_events.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerSendtojob : public CommandPlayerAction
	{
	private:
		const uint64_t value;

	public:
		explicit CommandPlayerSendtojob(CommandList* const parent, const uint64_t value, Label&& menu_name, std::vector<CommandName>&& command_names = {})
			: CommandPlayerAction(parent, std::move(menu_name), std::move(command_names)), value(value)
		{
		}

		void onClick(Click& click) final
		{
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
			{
				ensureScriptThread(thread_context, [this]
				{
					DEF_P2;
					for (const AbstractPlayer p : pp->getPlayers(pp->single))
					{
						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_SEND_TO_JOB),
							g_player,
							value
						};
						TSE(1 << p, args);
					}
				});
			});
		}
	};
}
