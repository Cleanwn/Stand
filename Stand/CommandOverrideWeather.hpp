#include "CommandListSelect.hpp"

#include "is_session.hpp"
#include "ManagedTunables.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "ScriptGlobal.hpp"
#include "weather_names.hpp"

namespace Stand
{
	class CommandOverrideWeather : public CommandListSelect
	{
	private:
		soup::Bytepatch sp_ground_snow_patch;

	public:
		explicit CommandOverrideWeather(CommandList* const parent)
			: CommandListSelect(parent, LOC("OVRDWTR"), CMDNAMES("weather", "myweather"), LOC("OVRDWTR_H"), {
				{-1, LOC("OVRDOFF"), CMDNAMES("normal")},
				{0, LOC("WTR_EXTRASUNNY"), CMDNAMES("extrasunny")},
				{1, LOC("WTR_CLEAR"), CMDNAMES("clear")},
				{2, LOC("WTR_CLOUDS"), CMDNAMES("clouds")},
				{3, LOC("WTR_SMOG"), CMDNAMES("smog")},
				{4, LOC("WTR_FOGGY"), CMDNAMES("foggy")},
				{5, LOC("WTR_OVERCAST"), CMDNAMES("overcast")},
				{6, LOC("WTR_RAIN"), CMDNAMES("rain")},
				{7, LOC("WTR_THUNDER"), CMDNAMES("thunder")},
				{8, LOC("WTR_CLEARING"), CMDNAMES("clearing")},
				{9, LOC("WTR_NEUTRAL"), CMDNAMES("neutral")},
				{10, LOC("WTR_SNOW"), CMDNAMES("snow")},
				{11, LOC("WTR_BLIZZARD"), CMDNAMES("blizzard")},
				{12, LOC("WTR_SNOWLIGHT"), CMDNAMES("snowlight")},
				{13, LOC("WTR_XMAS"), CMDNAMES("xmas")},
				{14, LOC("WTR_HALLOWEEN"), CMDNAMES("halloween")},
				{15, LOC("WTR_RAIN_HALLOWEEN"), CMDNAMES("rainhalloween")},
				{16, LOC("WTR_SNOW_HALLOWEEN"), CMDNAMES("snowhalloween")},
			}, -1)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (this->value == -1)
			{
				return;
			}

			if (this->value == 13 // xmas
				|| this->value == 16 // snow_halloween
				)
			{
				uint8_t patch[] = {
					soup::Bytepatch::JMP1
				};
				sp_ground_snow_patch.initPatch(pointers::sp_ground_snow_check, patch, COUNT(patch));
			}
			else
			{
				sp_ground_snow_patch.restore();
			}

			if (this->value == 13)
			{
				registerScriptTickEventHandler(click, [this]()
				{
					if (this->value == 13)
					{
						if (!is_session_transition_active(true))
						{
							ManagedTunables::snow.set(TRUE);
							ScriptGlobal(GLOBAL_UNK_GAME_STATE_XMAS).set<int>(0);
						}
						return true;
					}
					else
					{
						ManagedTunables::snow.reset();
						ScriptGlobal(GLOBAL_UNK_GAME_STATE_XMAS).set<int>(1);
						return false;
					}
				});
			}
			else
			{
				const auto value = this->value;
				const char* weather_name = weather_names[value];
				registerScriptTickEventHandler(click, [this, value, weather_name]()
				{
					if (this->value == value)
					{
						ManagedTunables::snow.set(FALSE);
						if (ManagedTunables::snow.has_real_value || !is_session_transition_active(true))
						{
							MISC::SET_OVERRIDE_WEATHER(weather_name);
						}
						return true;
					}
					else
					{
						ManagedTunables::snow.reset();
						MISC::CLEAR_OVERRIDE_WEATHER();
						return false;
					}
				});
			}
		}
	};
}
