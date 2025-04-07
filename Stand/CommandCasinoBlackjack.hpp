#pragma once

#include "CommandToggle.hpp"

#include "AntiAntiCheat.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"
#include "Tunables.hpp"

namespace Stand
{
	class CommandCasinoBlackjack : public CommandToggle
	{
	public:
		explicit CommandCasinoBlackjack(CommandList* const parent)
			: CommandToggle(parent, LOC("CSNO_BJ"), CMDNAMES("rigblackjack"), Label::combineWithSpace(LOC("CSNO_BJ_H"), LOC("CSNO_H_OPB")))
		{
		}

		void onChange(Click& click) final
		{
			if (m_on && g_tunables.getBool(TUNAHASH("DISABLE_BLACKJACK")))
			{
				click.setResponse(LOC("CMDDISA"));
				return onChangeImplUnavailable(click);
			}
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (!g_tunables.ready || g_tunables.getBool(TUNAHASH("DISABLE_BLACKJACK")))
				{
					return;
				}
				if (auto bj = GtaThread::fromHash(ATSTRINGHASH("blackjack")))
				{
					// The thread we just grabbed should be the only one for this script.
					SOUP_ASSERT(SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(ATSTRINGHASH("blackjack")) == 1);

					if (!bj->hostedByUser())
					{
						bj->snatchScriptHost();
					}
					else
					{
						AntiAntiCheat::disableScriptHostTracking();

						for (auto dealer_idx = 0; dealer_idx != *ScriptLocal(bj, LOCAL_BJ_DEALERCARDS).as<int*>(); ++dealer_idx)
						{
							for (auto card_idx = 0; card_idx != *ScriptLocal(bj, LOCAL_BJ_DEALERCARDS + 1 + (dealer_idx * LOCAL_BJ_DEALER_SIZE)).as<int*>(); ++card_idx)
							{
								if (*ScriptLocal(bj, LOCAL_BJ_DEALERCARDS + 1 + (dealer_idx * LOCAL_BJ_DEALER_SIZE) + 1 + card_idx).as<int*>() != 0)
								{
									// "card index" -> value -> dealer final value
									// 1 -> 11 -> 17
									// 47 -> 8 -> bust
									// 52 -> 10 -> 20
									*ScriptLocal(bj, LOCAL_BJ_DEALERCARDS + 1 + (dealer_idx * LOCAL_BJ_DEALER_SIZE) + 1 + card_idx).as<int*>() = 47;
								}
							}
						}
					}
				}
			});
		}
	};
}
