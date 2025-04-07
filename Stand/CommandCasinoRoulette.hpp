#pragma once

#include "CommandSlider.hpp"

#include "AntiAntiCheat.hpp"
#include "get_next_arg.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"
#include "Tunables.hpp"

#define ROULETTE_TABLE_MAX_NUMBER_OF_TABLES 6

namespace Stand
{
	class CommandCasinoRoulette : public CommandSlider
	{
	public:
		explicit CommandCasinoRoulette(CommandList* const parent)
			: CommandSlider(parent, LOC("ROULETTE"), CMDNAMES("rigroulette"), LOC("CSNO_H_OPB"), -1, 37, -1)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (allow_replacements)
			{
				if (num == -1)
				{
					return LANG_GET_W("RIGOFF");
				}
				if (num == 37)
				{
					return L"00";
				}
			}
			return CommandSlider::formatNumber(num);
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto og_args = args;
			auto arg = get_next_arg(args);
			if (arg == L"00")
			{
				std::wstring args_ = L"37";
				CommandSlider::onCommand(click, args_);
			}
			else
			{
				args = og_args;
				CommandSlider::onCommand(click, args);
			}
		}

		void onChange(Click& click, int prev_value) final
		{
			if (value != -1)
			{
				if (g_tunables.getBool(TUNAHASH("DISABLE_ROULETTE")))
				{
					click.setResponse(LOC("CMDDISA"));
					return onChangeImplUnavailable(click, prev_value);
				}
				registerScriptTickEventHandler(click, [this, value{ this->value }]
				{
					HANDLER_CHECK(value == this->value);
					if (g_tunables.ready)
					{
						if (g_tunables.getBool(TUNAHASH("DISABLE_ROULETTE")))
						{
							Click click(CLICK_BULK, TC_SCRIPT_NOYIELD);
							setValueIndicator(click, -1);
							HANDLER_BREAK;
						}
						if (auto roulette = GtaThread::fromHash(ATSTRINGHASH("casinoroulette")))
						{
							// The thread we just grabbed should be the only one for this script.
							SOUP_ASSERT(SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(ATSTRINGHASH("casinoroulette")) == 1);

							if (!roulette->hostedByUser())
							{
								roulette->snatchScriptHost();
							}
							else
							{
								AntiAntiCheat::disableScriptHostTracking();

								for (int i = 0; i != ROULETTE_TABLE_MAX_NUMBER_OF_TABLES; ++i)
								{
									*ScriptLocal(roulette, LOCAL_CR_BASE + LOCAL_CR_SPINRESULT + 1 + i).as<int*>() = value;
								}
							}
						}
					}
					HANDLER_END;
				});
			}
		}
	};
}
