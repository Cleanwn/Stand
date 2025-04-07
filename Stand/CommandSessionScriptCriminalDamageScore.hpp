#pragma once

#include "CommandSlider.hpp"

#include "AbstractPlayer.hpp"

namespace Stand
{
	class CommandSessionScriptCriminalDamageScore : public CommandSlider
	{
	public:
		explicit CommandSessionScriptCriminalDamageScore(CommandList* const parent)
			: CommandSlider(parent, LOC("SCORE"), CMDNAMES_OBF("criminaldamagescore"), NOLABEL, 0, INT_MAX, 0, 1, CMDFLAGS_SLIDER | CMDFLAG_TEMPORARY)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			if (click.isUltimateEdition())
			{
				const auto val = (int)value;
				ensureYieldableScriptThread(click, [this, val]
				{
					if (auto thread = parent->as<CommandSessionScript>()->getThreadAndEnsureControl())
					{
						*ScriptLocal(thread, LOCAL_CRIMDMG_SCORE).as<int*>() = val;
						*ScriptLocal(thread, LOCAL_CRIMDMG_PLYDATA).at(g_player, LOCAL_CRIMDMG_PLYDATA_SIZE).at(3).as<int*>() = val;
					}
				});
			}
			else
			{
				onChangeImplUnavailable(click, prev_value);
			}
		}

		void onTickInGameViewport() final
		{
			if (auto thread = parent->as<CommandSessionScript>()->getThread())
			{
				Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
				setValueIndicator(click, *ScriptLocal(thread, LOCAL_CRIMDMG_PLYDATA).at(g_player, LOCAL_CRIMDMG_PLYDATA_SIZE).at(3).as<int*>());
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void applyDefaultState() final
		{
		}
	};
}
