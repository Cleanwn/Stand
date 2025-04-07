#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandSessionScriptChallengesType : public CommandListSelect
	{
	public:
		explicit CommandSessionScriptChallengesType(CommandList* const parent)
			: CommandListSelect(parent, LOC("TYP"), {}, NOLABEL, {
				{0, LOC("SCR_FE_C_0")},
				{1, LOC("SCR_FE_C_1")},
				{2, LOC("SCR_FE_C_2")},
				{3, LOC("SCR_FE_C_3")},
				{4, LOC("SCR_FE_C_4")},
				{5, LOC("SCR_FE_C_5")},
				{6, LOC("SCR_FE_C_6")},
				{7, LOC("SCR_FE_C_7")},
				{8, LOC("SCR_FE_C_8")},
				{9, LOC("SCR_FE_C_9")},
				{10, LOC("SCR_FE_C_10")},
				{11, LOC("SCR_FE_C_11")},
				{12, LOC("SCR_FE_C_12")},
				{13, LOC("SCR_FE_C_13")},
				{14, LOC("SCR_FE_C_14")},
				{15, LOC("SCR_FE_C_15")},
				{16, LOC("SCR_FE_C_16")},
				{17, LOC("SCR_FE_C_17")},
				{18, LOC("SCR_FE_C_18")},
			}, 0, CMDFLAGS_LIST_SELECT | CMDFLAG_TEMPORARY)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (!click.isAuto())
			{
				if (click.isUltimateEdition())
				{
					const auto val = (int)value;
					ensureYieldableScriptThread(click, [this, val]
					{
						if (auto thread = parent->as<CommandSessionScript>()->getThreadAndEnsureControl())
						{
							*ScriptLocal(thread, LOCAL_CHLNG_SHDATA + 3).as<int*>() = val;
						}
					});
				}
				else
				{
					onChangeImplUnavailable(click, prev_value);
				}
			}
		}

		void onTickInGameViewport() final
		{
			if (auto thread = parent->as<CommandSessionScript>()->getThread())
			{
				if (isFocused())
				{
					// If the game is running at a really high framerate, this can cause this->value to be overwritten before the onChange handler had a chance to read it, causing the change to fail.
				}
				else
				{
					setValueIndication(*ScriptLocal(thread, LOCAL_CHLNG_SHDATA + 3).as<int*>());
				}
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
