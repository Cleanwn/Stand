#pragma once

#include "CommandToggle.hpp"

#include "NativeTableHooks.hpp"
#ifdef STAND_DEBUG
#include "Util.hpp"
#endif

namespace Stand
{
	class CommandNoBusinessBattleMusic : public CommandToggle
	{
	private:
		static constexpr const char* events[] = {
			// Business Battles 1
			"BTL_IDLE_START",
			"BTL_MED_INTENSITY_START",
			"BTL_GUNFIGHT_START",
			"BTL_SUSPENSE",
			"BTL_MED_INTENSITY",
			"BTL_DELIVERING",
			"BTL_GUNFIGHT",
			"BTL_VEHICLE_ACTION",
			"BTL_DELIVERING_START",
			"BTL_MUSIC_STOP",
			"BTL_FAIL",

			// Business Battles 2
			"SUM20_BB_FAIL",
			"SUM20_BB_CAPTURED_STOP",
			"SUM20_BB_CAPTURED_START",
			"SUM20_BB_CAPTURED_APPROACH",
			"SUM20_BB_CAPTURED_HANGAR",
			"SUM20_BB_CAPTURED_DELIVERING",
			"SUM20_BB_AIRCRAFT_STOP",
			"SUM20_BB_AIRCRAFT_START",
			"SUM20_BB_AIRCRAFT_COLLECT",
			"SUM20_BB_AIRCRAFT_COMBAT",
			"SUM20_BB_AIRCRAFT_DELIVERING",
			"SUM20_BB_FACTORY_STOP",
			"SUM20_BB_FACTORY_START",
			"SUM20_BB_FACTORY_HACK_START",
			"SUM20_BB_FACTORY_HACK_END",
			"SUM20_BB_FACTORY_COMBAT",
			"SUM20_BB_FACTORY_DELIVERING",
		};

		static void TRIGGER_MUSIC_EVENT(rage::scrNativeCallContext& ctx)
		{
			const auto name = ctx.getArg<const char*>(0);

			for (auto& music : events)
			{
				if (strcmp(name, music) == 0)
				{
#ifdef STAND_DEBUG
					Util::toast(fmt::format("Disallowing business battle music event: {}", name), TOAST_ALL);
#endif
					return;
				}
			}

#ifdef STAND_DEBUG
			Util::toast(fmt::format("Allowing business battle music event: {}", name), TOAST_ALL);
#endif

			ctx.invokePassthrough(0x706D57B0F50DA710);
		}

	public:
		explicit CommandNoBusinessBattleMusic(CommandList* const parent)
			: CommandToggle(parent, LOC("NOFMBTLMSC"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAMES("businessbattlemusic")), LOC("NOFMBTLMSC_H"))
		{
		}

		void onEnable(Click& click) final
		{
			NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("BUSINESS_BATTLES"), 0x706D57B0F50DA710, &TRIGGER_MUSIC_EVENT);
			NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("fm_content_business_battles"), 0x706D57B0F50DA710, &TRIGGER_MUSIC_EVENT);
		}

		void onDisable(Click& click) final
		{
			NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("BUSINESS_BATTLES"), 0x706D57B0F50DA710);
			NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("fm_content_business_battles"), 0x706D57B0F50DA710);
		}
	};
}
