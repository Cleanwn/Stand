#pragma once

#include "CommandListSelect.hpp"

#include "FiberPool.hpp"
#include "NativeTableHooks.hpp"
#include "StringUtils.hpp"
#include "Tunables.hpp"

namespace Stand
{
	class CommandCasinoSlots : public CommandListSelect
	{
	public:
		explicit CommandCasinoSlots(CommandList* const parent)
			: CommandListSelect(parent, LOC("SLOTS2"), CMDNAMES("rigslotmachines"), LIT(LANG_FMT("SLOTS_H", StringUtils::toStringWithThousandsSeparator(g_tunables.getInt(TUNAHASH("slots threshold help text"))))), {
				{-1, LOC("RIGOFF"), CMDNAMES("off")},
				{63, LOC("SLOTS_WJ"), CMDNAMES("jackpot")},
				{0, LOC("SLOTS_L"), CMDNAMES("loss")},
				{2, LOC("SLOTS_WS"), CMDNAMES("smallwin")},
			}, -1)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (g_tunables.getBool(TUNAHASH("DISABLE_SLOTS")) && value != -1)
			{
				click.setResponse(LOC("CMDDISA"));
				return onChangeImplUnavailable(click, prev_value);
			}
			ensureScriptThread(click, [this]
			{
				rig = value;
				if (value != -1)
				{
					NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("casino_slots"), 0xF2D49816A804D134, &_GET_RANDOM_INT_IN_RANGE_2);
				}
				else
				{
					NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("casino_slots"), 0xF2D49816A804D134);
				}
			});
		}

	private:
		inline static long long rig = 0;

		static void _GET_RANDOM_INT_IN_RANGE_2(rage::scrNativeCallContext& ctx)
		{
			// Ensure that 'MPPLY_CASINO_CHIPS_WON_GD' is (more or less) correctly tracked.
			ScriptGlobal(GLOBAL_CASINO_MAX_LOSS).set<int>(50'000'000);

			if (rig != -1 && ctx.getArg<int>(1) == 64)
			{
				if (rig != 0)
				{
					int won = -1;
					STATS::STAT_GET_INT(ATSTRINGHASH("MPPLY_CASINO_CHIPS_WON_GD"), (int*)&won, -1);
					if (won >= g_tunables.getInt(TUNAHASH("slots threshold"))
						|| g_tunables.getBool(TUNAHASH("DISABLE_SLOTS"))
						)
					{
						rig = 0;
					}
				}
				ctx.setReturnValue<>(rig);
			}
			else
			{
				ctx.invokePassthrough(0xF2D49816A804D134);
			}
		}
	};
}
