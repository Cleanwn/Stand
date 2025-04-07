#pragma once

#include "CommandListSelect.hpp"

#include <fmt/core.h>

#include "is_session.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "ScriptGlobal.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandCasinoWheel : public CommandListSelect
	{
	private:
		static int func_123(int iParam0)
		{
			switch (iParam0)
			{
			case 0:
				return 6;

			case 1:
				return 5;

			case 2:
				return 4;

			case 3:
				return 3;

			case 4:
				return 2;

			case 5:
				switch (func_641())
				{
				case 0:
					return 13;

				case 1:
					return 7;

				case 2:
					return 10;

				case 3:
					return 11;

				case 4:
					return 12;

				case 5:
					return 9;

				case 6:
					return 8;
				}
				break;

			case 6:
				return 1;
			}
			return 0;
		}

		static int func_805(int iParam0)
		{
			switch (iParam0)
			{
			case 0:
				return 0;

			case 1:
				return 8;

			case 2:
				return 12;

			case 3:
				return 16;

			case 4:
				return 1;

			case 5:
				return 5;

			case 6:
				return 9;

			case 7:
				return 13;

			case 8:
				return 17;

			case 9:
				return 2;

			case 10:
				return 6;

			case 11:
				return 14;

			case 12:
				return 19;

			case 13:
				return 3;

			case 14:
				return 7;

			case 15:
				return 10;

			case 16:
				return 15;

			case 17:
				return 11;

			case 18:
				return 18;

			case 19:
				return 4;
			}
			return 0;
		}

		static int func_806()
		{
			int iVar0[20];
			int iVar2;
			int iVar3;

			iVar2 = 0;
			while (iVar2 < 20)
			{
				iVar0[iVar2] = iVar2;
				iVar2++;
			}
			iVar3 = invoke<int>(&_GET_RANDOM_INT_IN_RANGE_2, 0, lucky_wheel_1_max);
			iVar2 = 0;
			while (iVar2 < 20)
			{
				if (iVar3 <= _1_51_func_832(iVar0[iVar2]))
				{
					return iVar0[iVar2];
				}
				else
				{
					iVar3 = (iVar3 - _1_51_func_832(iVar0[iVar2]));
				}
				iVar2++;
			}
			return -1;
		}

		static int _1_51_func_832(int iParam0)
		{
			switch (iParam0)
			{
			case 0:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 0).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 0).as<int*>();
				}
				break;

			case 1:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 0).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 1).as<int*>();
				}
				break;

			case 2:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 0).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 2).as<int*>();
				}
				break;

			case 3:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 0).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 3).as<int*>();
				}
				break;

			case 4:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 1).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 4).as<int*>();
				}
				break;

			case 5:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 1).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 5).as<int*>();
				}
				break;

			case 6:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 1).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 6).as<int*>();
				}
				break;

			case 7:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 1).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 7).as<int*>();
				}
				break;

			case 8:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 1).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 8).as<int*>();
				}
				break;

			case 9:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 2).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 9).as<int*>();
				}
				break;

			case 10:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 2).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 10).as<int*>();
				}
				break;

			case 11:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 2).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 11).as<int*>();
				}
				break;

			case 12:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 2).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 12).as<int*>();
				}
				break;

			case 13:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 3).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 13).as<int*>();
				}
				break;

			case 14:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 3).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 14).as<int*>();
				}
				break;

			case 15:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 3).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 15).as<int*>();
				}
				break;

			case 16:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 3).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 16).as<int*>();
				}
				break;

			case 17:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 4).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 17).as<int*>();
				}
				break;

			case 18:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 5).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 18).as<int*>();
				}
				break;

			case 19:
				if (*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE + 6).as<int*>())
				{
					return 0;
				}
				else
				{
					return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE + 19).as<int*>();
				}
				break;
			}
			return 0;
		}

		static  int func_124(int iVar0)
		{
			switch (iVar0)
			{
			case 1:
				return 1;

			case 2:
				return 2;

			case 3:
				return 3;

			case 0:
				return 0;

			case 5:
				return 1;

			case 7:
				return 3;

			case 11:
				return 5;

			case 8:
				return 0;

			case 9:
				return 1;

			case 6:
				return 2;

			case 10:
				return 3;

			case 12:
				return 0;

			case 13:
				return 1;

			case 18:
				return 6;

			case 14:
				return 2;

			case 16:
				return 0;

			case 17:
				return 1;

			case 19:
				return 2;

			case 15:
				return 3;

			case 4:
				return 4;
			}
			return -1;
		}


		static int func_641()
		{
			int iVar0[7];
			int iVar2;
			int iVar3;

			iVar2 = 0;
			while (iVar2 < 7)
			{
				iVar0[iVar2] = iVar2;
				iVar2++;
			}
			iVar3 = invoke<int>(&_GET_RANDOM_INT_IN_RANGE_2, 0, lucky_wheel_2_max);
			iVar2 = 0;
			while (iVar2 < 7)
			{
				if (iVar3 <= _1_51_func_678(iVar0[iVar2]))
				{
					return iVar0[iVar2];
				}
				else
				{
					iVar3 = (iVar3 - _1_51_func_678(iVar0[iVar2]));
				}
				iVar2++;
			}
			return -1;
		}

		static int _1_51_func_678(int iParam0)
		{
			switch (iParam0)
			{
			case 0:
				return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_2_BASE + 0).as<int*>();

			case 1:
				return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_2_BASE + 1).as<int*>();

			case 2:
				return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_2_BASE + 2).as<int*>();

			case 3:
				return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_2_BASE + 3).as<int*>();

			case 4:
				return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_2_BASE + 4).as<int*>();

			case 5:
				return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_2_BASE + 5).as<int*>();

			case 6:
				return *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_2_BASE + 6).as<int*>();
			}
			return 0;
		}

		bool hooked = false;

	public:
		explicit CommandCasinoWheel(CommandList* const parent)
			: CommandListSelect(parent, LOC("WHEEL"), CMDNAMES("rigluckywheel"), NOLABEL, {
				{0, LOC("RIGOFF"), CMDNAMES("off")},
				{1, LOC("WHEEL_1"), CMDNAMES("vehicle", "podiumvehicle")},
				{2, LOC("WHEEL_2"), CMDNAMES("discount")},
				{3, LOC("WHEEL_3"), CMDNAMES("chips")},
				{4, LOC("WHEEL_4"), CMDNAMES("cash")},
				{5, LIT("RP"), CMDNAMES("rp")},
				{6, LOC("WHEEL_6"), CMDNAMES("clothing")},
				{7, LOC("WHEEL_7"), CMDNAMES("mysteryvehicle")},
				{8, LOC("WHEEL_8"), CMDNAMES("supplies")},
				{9, LOC("WHEEL_9"), CMDNAMES("snacks")},
				{10, LOC("WHEEL_10"), CMDNAMES("mysterychips")},
				{11, LOC("WHEEL_11"), CMDNAMES("mysterycash")},
				{12, LOC("WHEEL_12"), CMDNAMES("mysteryrp")},
				{13, LOC("WHEEL_13"), CMDNAMES("mysteryclothing")},
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			ensureScriptThread(click, [=]
			{
				if (value != 0)
				{
					if (is_session_started())
					{
						int i = 0;
						lucky_wheel_1_max = 0;
						while (i < 20)
						{
							lucky_wheel_1_max += _1_51_func_832(i++);
						}
						i = 0;
						lucky_wheel_2_max = 0;
						while (i < 7)
						{
							lucky_wheel_2_max += _1_51_func_678(i++);
						}
						lucky_wheel_1_win = 0;
						do
						{
							lucky_wheel_2_win = 0;
							do
							{
								if (func_123(func_124(func_805(func_806()))) == this->value)
								{
									NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("casino_lucky_wheel"), 0xF2D49816A804D134, &_GET_RANDOM_INT_IN_RANGE_2);
									hooked = true;
									//util::toast(std::string("RNG determined: ").append(fmt::to_string(g_hooking.lucky_wheel_1_win).append(" - ").append(fmt::to_string(g_hooking.lucky_wheel_2_win))));
									return;
								}
							} while (++lucky_wheel_2_win < lucky_wheel_2_max);
						} while (++lucky_wheel_1_win < lucky_wheel_1_max);
						Util::toast(std::string("Failed to determine RNG"));
					}
					else
					{
						Util::toast(LOC("CMDONL"));
					}
					Click click(CLICK_BULK, TC_SCRIPT_NOYIELD);
					setState(click, LANG_GET_EN("RIGOFF"));
				}
				if (hooked)
				{
					hooked = false;
					NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("casino_lucky_wheel"), 0xF2D49816A804D134);
				}
			});
		}

	private:
		inline static int lucky_wheel_1_max = 100;
		inline static int lucky_wheel_1_win = 92;
		inline static int lucky_wheel_2_max = 350;
		inline static int lucky_wheel_2_win = 0;

		static void _GET_RANDOM_INT_IN_RANGE_2(rage::scrNativeCallContext& ctx)
		{
			const int max = ctx.getArg<int>(1);
			if (max == lucky_wheel_1_max)
			{
				ctx.setReturnValue<>(lucky_wheel_1_win);
				return;
			}
			if (max == lucky_wheel_2_max)
			{
				ctx.setReturnValue<>(lucky_wheel_2_win);
				return;
			}
			ctx.invokePassthrough(0xF2D49816A804D134);
		}
	};
}
