#pragma once

#include "CommandSliderDynamic.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Util.hpp"
#include "VehicleMods.hpp"

namespace Stand
{
	class CommandVehmodInt : public CommandSliderDynamic
	{
	private:
		const int modType;

	public:
		explicit CommandVehmodInt(CommandList* const parent, const int modType)
			: CommandSliderDynamic(parent, VehicleMods::getName(modType), { {} }, NOLABEL, -1, -1, -1), modType(modType)
		{
			switch (modType)
			{
			default:
				useCommandNamesFromMenuName();
				break;

			case VehicleMods::front_bumper:
				command_names = { CMDNAME("frontbumper"), CMDNAME("countermeasures") };
				break;

			case VehicleMods::roof:
				command_names = { CMDNAME("roof"), CMDNAME("weapons") };
				break;

			case VehicleMods::armor:
				command_names = { CMDNAME("armour"), CMDNAME("armor") };
				break;
			}
		}

		void onChange(Click& click, int prev_value) final
		{
			if (!click.isStand())
			{
				ensureScriptThread(click, [this, click]() mutable
				{
					auto veh = Util::getVehicle(click);
					if (veh.isValid())
					{
						veh.applyUpgrade(modType, value);

						// force front and back wheels to sync, later on I will decouple the front and back wheels.
						if (auto const veh_class = VEHICLE::GET_VEHICLE_CLASS(veh); veh_class == 8 && modType == VehicleMods::front_wheels)
						{
							VEHICLE::SET_VEHICLE_MOD(veh, VehicleMods::back_wheels, value, false);
						}
					}
				});
			}
		}

		void onTickInGameViewport() final
		{
			auto veh = Util::getVehicle();
			Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
			setMaxValue(veh.getUpgradeMaxValue(modType));
			setValue(click, veh.getUpgradeValue(modType));
			
			if (parent->isT<CommandListConcealer>())
			{
				parent->as<CommandListConcealer>()->setVisible(this, CommandCustomsShowNa::instance->m_on || !isNotApplicable());
			}
		}
	};
}
