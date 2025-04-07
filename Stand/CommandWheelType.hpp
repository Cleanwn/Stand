#pragma once

#include "CommandSlider.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Util.hpp"
#include "AbstractPlayer.hpp"

namespace Stand
{
	class CommandWheelType : public CommandSlider
	{
		enum WheelTypes
		{
			wheelTypeSport,
			wheelTypeMuscle,
			wheelTypeLowrider,
			wheelTypeSuv,
			wheelTypeOffroad,
			wheelTypeTuner,
			wheelTypeBike,
			wheelTypeHighEnd,
			wheelTypeBennys,
			wheelTypeBennysBespoke,
			wheelTypeRace,
			wheelTypeStreet,
			wheelTypeTrack,
		};

		static constexpr int car_wheel_types[]
		{
			wheelTypeSport,
			wheelTypeMuscle,
			wheelTypeLowrider,
			wheelTypeSuv,
			wheelTypeOffroad,
			wheelTypeTuner,
			wheelTypeHighEnd,
			wheelTypeBennys,
			wheelTypeBennysBespoke,
			wheelTypeRace,
			wheelTypeStreet,
			wheelTypeTrack,
		};

		static constexpr int motorcycle_wheel_types[]
		{
			wheelTypeBike,
		};

		bool on_motorcycle {};

		int const* wheel_type_indexes = car_wheel_types;

		int get_current_wheel_index(AbstractEntity veh) const
		{
			return std::max(0, std::min(veh.getUpgradeValue(VehicleMods::front_wheels), veh.getUpgradeMaxValue(VehicleMods::front_wheels) - 1));
		}

		int array_index_from_wheel_type(int const wheel_type) const
		{
			if(auto* const result = std::find(std::begin(car_wheel_types), std::end(car_wheel_types), wheel_type);
				result != std::end(car_wheel_types))
			{
				return static_cast<int>(result - std::begin(car_wheel_types));
			}

			return 0;
		}

		int get_current_wheel_type_index(AbstractEntity veh) const
		{
			if(!veh.isValid() || on_motorcycle)
			{
				return 0;
			}

			return array_index_from_wheel_type(VEHICLE::GET_VEHICLE_WHEEL_TYPE(veh));
		}

		void update_vehicle_wheels(AbstractEntity veh) const
		{
			VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

			VEHICLE::SET_VEHICLE_WHEEL_TYPE(veh, wheel_type_indexes[value]);

			auto const wheel_index = get_current_wheel_index(veh);
			
			veh.applyUpgrade(VehicleMods::front_wheels, wheel_index);

			if (on_motorcycle)
			{
				veh.applyUpgrade(VehicleMods::back_wheels, wheel_index);
			}
		}

	public:
		explicit CommandWheelType(CommandList* const parent)
			: CommandSlider(parent, LOC("WHLTYP"), CMDNAMES("wheeltype"), NOLABEL, 0, 0, 0, 1, CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!isInViewport())
			{
				onTickInGameViewport();
			}

			return CommandSlider::onCommand(click, args);
		}

		void onChange(Click& click, int prev_value) final
		{
			if(!click.isStand())
			{
				ensureScriptThread(click, [this, click]() mutable
				{
					if(auto veh = Util::getVehicle(click); veh.isValid())
					{
						update_vehicle_wheels(veh);
					}
				});
			}
		}

		void onTickInGameViewport() final
		{
			if(auto veh = Util::getVehicle(); veh.isValid())
			{
				Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);

				if(VEHICLE::GET_VEHICLE_CLASS(veh) != 8)
				{
					setValue(click, get_current_wheel_type_index(veh));
					setMaxValue(COUNT(car_wheel_types) - 1);

					wheel_type_indexes = car_wheel_types;
					on_motorcycle = false;
				}
				else
				{
					setMaxValue(0);

					wheel_type_indexes = motorcycle_wheel_types;
					on_motorcycle = true;
				}
			}
			else
			{
				setMaxValue(0);
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
