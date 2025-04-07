#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "script_events.hpp"
#include "tse.hpp"

namespace Stand
{
	class CommandPlayerInfiniteLoading : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerInfiniteLoading(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("INFLOAD"), CMDNAMES("infiniteloading"))
		{
		}

		void onClick(Click& click) final
		{
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this, click](ThreadContext thread_context)
			{
				ensureScriptThread(thread_context, [this, click]
				{
					for (const auto& p : PP_PTR->getPlayers())
					{
						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_APARTMENT_INVITE),
							g_player, // 1: Sender
							click.issuer_is_explicit ? uint64_t(click.issued_for) : uint64_t(p), // 2: Owner
							Util::i32_to_u64_trickery(-1),
							1,
							AptHelper::max_property_id + 1, // 5: Property ID
							0,
							0,
							0,
							0,
						};
						TSE(1 << p, args);
					}
				});
			});
		}
	};
}
