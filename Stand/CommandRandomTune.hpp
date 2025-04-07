#pragma once

#include "CommandTextslider.hpp"

#include <soup/rand.hpp>

namespace Stand
{
	class CommandRandomTune : public CommandTextslider
	{
	public:
		enum TuneType : uint8_t
		{
			ALL,
			VISUAL,
			PERFORMANCE,
			PERFORMANCE_SPOILER,
		};

	private:
		[[nodiscard]] bool shouldUpgrade(int mod_type, TuneType tune_type) const
		{
			switch (mod_type)
			{
			case 11: // Engine
			case 12: // Brakes
			case 13: // Transmission
			case 15: // Suspension
			case 16: // Armor
			case 18: // Turbo
				return tune_type != VISUAL;
			}

			if (mod_type == 0 && tune_type != PERFORMANCE)
			{
				return true;
			}

			return tune_type < PERFORMANCE; // All remaining upgrades are aesthetic.
		}

	public:
		explicit CommandRandomTune(CommandList* const parent)
			: CommandTextslider(parent, LOC("RTUNE"), CMDNAMES_OBF("randomtune"), NOLABEL, {
				{ ALL, LOC("ALL") },
				{ VISUAL, LOC("RTUNE_VIS") },
				{ PERFORMANCE, LOC("RTUNE_PERF") },
				{ PERFORMANCE_SPOILER, LOC("RTUNE_PERF_S") },
			})
		{
		}

		void onClick(Click& click) final
		{
			ensureScriptThread(click, [&click, this]
			{
				if (const Vehicle veh = Util::getVehicle(click))
				{
					AbstractEntity::get(veh).requestControl(ATSTRINGHASH("tune"), [this](AbstractEntity& veh)
					{
						auto tune_type = (TuneType)this->value;
						VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

						if (tune_type != VISUAL)
						{
							VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, soup::rand.coinflip());
						}

						if (tune_type < PERFORMANCE)
						{
							VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, soup::rand.t<int>(0, 255), soup::rand.t<int>(0, 255), soup::rand.t<int>(0, 255));
							VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, soup::rand.t<int>(0, 255), soup::rand.t<int>(0, 255), soup::rand.t<int>(0, 255));
						}

						for (const auto& modType : VehicleMods::all)
						{
							if (shouldUpgrade(modType, tune_type))
							{
								veh.applyUpgrade(modType, soup::rand.t<int>(-1, veh.getUpgradeMaxValue(modType)));
							}
						}
					}, [](AbstractEntity&)
					{
						Util::toast(LOC("VEHCTRL"));
					});
				}
			});
		}
	};
}
