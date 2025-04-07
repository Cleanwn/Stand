#pragma once

#include "CommandPlayerAction.hpp"

#include "AbstractPlayer.hpp"

namespace Stand
{
	class CommandPlayerRp : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerRp(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("PLYRP"), { CMDNAME("rp") }, LOC("PLYRP_H"), COMMANDPERM_NEUTRAL)
		{
		}

		void onClick(Click& click) final
		{
			ensureYieldableScriptThread(click, [this]
			{
				if (soup::rand.coinflip())
				{
					for (uint8_t i = 0; i != 20; ++i)
					{
						for (const auto& p : PP_PTR->getPlayers())
						{
							// This event gives 1,000 RP every time it's sent.
							// It used to also have a chance to turn them into a random animal, but no more. :(
							uint64_t args[] = {
								Util::i32_to_u64_trickery(SE_GIVE_COLLECTIBLE),
								g_player,
								8, // Jack O' Lanterns
								(uint64_t)(int64_t)-5, // bypasses collected check
								1,
								1,
								1
							};
							TSE(1 << p, args);
						}
						Script::current()->yield(200);
					}
				}
				else
				{
					for (uint8_t i = 20; i != 24 + 1; ++i)
					{
						for (const auto& p : PP_PTR->getPlayers())
						{
							// This event gives 1,000 RP every time it's sent.
							uint64_t args[] = {
								Util::i32_to_u64_trickery(SE_GIVE_COLLECTIBLE),
								g_player,
								4,
								i,
								1,
								1,
								1
							};
							TSE(1 << p, args);
							Script::current()->yield(200);
						}
					}
				}
			});
		}
	};
}
