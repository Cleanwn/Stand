#include "CommandCustoms.hpp"

#include "CommandDivider.hpp"
#include "CommandListConcealer.hpp"
#include "CommandListViewport.hpp"
#include "CommandRainbow.hpp"
#include "CommandToggleAll.hpp"
#include "CommandTogglePointer.hpp"
#include "create_rainbow.hpp"

#include "CommandVehicleModelFlag.hpp"
#include "CommandVehicleParachuteModel.hpp"

#include "CommandPlateType.hpp"
#include "CommandPlate.hpp"
#include "CommandPlateRandomise.hpp"
#include "CommandInputScrollingText.hpp"
#include "CommandScrollingPlateText.hpp"
#include "CommandPlateSpeed.hpp"
#include "CommandPlateTps.hpp"
#include "CommandLockPlate.hpp"

#include "CommandVehfinish.hpp"

#include "CommandVehcolourPrimary.hpp"
#include "CommandVehcolourSecondary.hpp"
#include "CommandVehcolourTire.hpp"
#include "CommandVehcolourNeons.hpp"
#include "CommandStandardGameVehicleColour.hpp"
#include "CommandInteriorColour.hpp"
#include "CommandIndependenceTires.hpp"

#include "CommandListVehicleExtras.hpp"
#include "CommandVehicleExtra.hpp"

#include "CommandScorched.hpp"
#include "CommandTonk.hpp"

#include "CommandCustomsShowNa.hpp"

#include "CommandVehmodBool.hpp"
#include "CommandVehmodInt.hpp"
#include "CommandHeadlights.hpp"
#include "CommandWindowTint.hpp"
#include "CommandVehRoofLivery.hpp"
#include "CommandVehDriftTyres.hpp"
#include "CommandVehBulletproofTyres.hpp"
#include "CommandWheelType.hpp"
#include "CommandWheelColour.hpp"
#include "CommandNeonLight.hpp"

#include "CommandTune.hpp"
#include "CommandPerf.hpp"
#include "CommandPerfWithSpoiler.hpp"
#include "CommandRandomTune.hpp"
#include "CommandUntune.hpp"

namespace Stand
{
	CommandCustoms::CommandCustoms(CommandList* const parent)
		: CommandList(parent, LOC("LSC"), CMDNAMES("lsc", "customs", "lossantoscustoms", "losantoscustoms"))
	{
		// Performance
		{
			auto perf = this->createChild<CommandListViewport<CommandListConcealer>>(LOC("TUNE_PERF"));

			perf->createChild<CommandVehmodInt>(VehicleMods::armor);
			perf->createChild<CommandVehmodInt>(VehicleMods::brakes);
			perf->createChild<CommandVehmodInt>(VehicleMods::engine);
			perf->createChild<CommandVehmodInt>(VehicleMods::spoiler);
			perf->createChild<CommandVehmodInt>(VehicleMods::transmission);
			perf->createChild<CommandVehmodBool>(VehicleMods::turbo);
		}

		// Appearance
		{
			auto appearance = this->createChild<CommandListViewport<CommandListConcealer>>(LOC("VIS"));

			// Primary Colour
			CommandVehcolour* primary_colour;
			{
				auto _ = appearance->createChildWithBuilder<CommandVehcolourPrimary>();
				primary_colour = _;
				primary_colour->createChild<CommandRainbow>(LOC("LGBT"), CMDNAMES("vehprimaryrainbow", "vehicleprimaryrainbow"), primary_colour);
				primary_colour->createChild<CommandVehfinish>(primary_colour, LOC("CLRFIN"), CMDNAMES("vehprimaryfinish"));
				primary_colour->createChild<CommandStandardGameVehicleColour>(LOC("CLRSTD"), CMDNAMES("vehprimarylsc"), CommandStandardGameVehicleColour::PRIMARY);
			}
			
			// Secondary Colour
			{
				auto secondary_colour = appearance->createChild<CommandVehcolourSecondary>();
				secondary_colour->createChild<CommandRainbow>(LOC("LGBT"), CMDNAMES("vehsecondaryrainbow", "vehiclesecondaryrainbow"), secondary_colour);
				secondary_colour->createChild<CommandVehfinish>(secondary_colour, LOC("CLRFIN"), CMDNAMES("vehsecondaryfinish"));
				secondary_colour->createChild<CommandStandardGameVehicleColour>(LOC("CLRSTD"), CMDNAMES("vehsecondarylsc"), CommandStandardGameVehicleColour::SECONDARY);
				secondary_colour->populate(primary_colour);
			}

			appearance->createChild<CommandStandardGameVehicleColour>(LOC("CLRPRL"), CMDNAMES("vehpearlescent"), CommandStandardGameVehicleColour::PEARLESCENT);
			appearance->createChild<CommandInteriorColour>(LOC("CLRINT"), CMDNAMES("vehinteriorcolour"), CommandStandardGameVehicleColour::INTERIOR);

			// Neons
			{
				auto neons = appearance->createChild<CommandList>(LOC("NEONLGHTS"));
				CommandIssuable::collapse_command_names.emplace(L"vehneon");

				create_rainbow(neons->createChildWithBuilder<CommandVehcolourNeons>());

				neons->createChild<CommandToggleAll>(CMDNAMES("vehneonall"));
				neons->createChild<CommandNeonLight>(2, LOC("APOS_F"), CMDNAMES("vehneonfront"));
				neons->createChild<CommandNeonLight>(3, LOC("APOS_B"), CMDNAMES("vehneonback"));
				neons->createChild<CommandNeonLight>(0, LOC("RPOS_L"), CMDNAMES("vehneonleft"));
				neons->createChild<CommandNeonLight>(1, LOC("RPOS_R"), CMDNAMES("vehneonright"));
			}

			// Wheel mods
			{
				auto wheels = appearance->createChild<CommandList>(LOC("WHLS"));

				wheels->createChild<CommandWheelType>();
				wheels->createChild<CommandVehmodInt>(VehicleMods::front_wheels);
				wheels->createChild<CommandWheelColour>();

				// Tyre smoke
				{
					auto tyre_smoke_colour = wheels->createChild<CommandVehcolourTire>();
					tyre_smoke_colour->children.emplace_back(std::unique_ptr<CommandRainbow>(new CommandRainbow(tyre_smoke_colour, LOC("LGBT"), CMDNAMES("vehtirerainbow", "vehicletirerainbow"), tyre_smoke_colour)));
					tyre_smoke_colour->createChild<CommandIndependenceTires>();
					tyre_smoke_colour->populate(primary_colour);
				}

				wheels->createChild<CommandVehDriftTyres>();
				wheels->createChild<CommandVehBulletproofTyres>();
			}
			
			// Plate mods
			{
				auto plate = appearance->createChild<CommandList>(LOC("PLATE"));

				plate->createChild<CommandPlateType>();
				auto cplate = plate->createChild<CommandPlate>();
				plate->createChild<CommandPlateRandomise>();

				// Scrolling plate text
				{
					auto scrolling_plate = plate->createChild<CommandList>(LOC("SCRLTXT"));

					auto scrolling_plate_text = scrolling_plate->makeChild<CommandInputScrollingText>();
					auto scrolling_plate_delay = scrolling_plate->makeChild<CommandSlider>(LOC("INTVAL_MS"), CMDNAMES("platescrolldelay"), NOLABEL, 0, 60000, 250, 25);
					
					scrolling_plate->createChild<CommandScrollingPlateText>(scrolling_plate_text.get(), scrolling_plate_delay.get());
					scrolling_plate->children.push_back(std::move(scrolling_plate_text));
					scrolling_plate->children.push_back(std::move(scrolling_plate_delay));
				}

				plate->createChild<CommandPlateSpeed>();
				plate->createChild<CommandPlateTps>();
				plate->createChild<CommandLockPlate>(cplate);
			}
			
			// Parachute
			{
				auto parachute = appearance->createChild<CommandList>(LOC("PARA"));

				parachute->createChild<CommandVehicleModelFlag>(CVehicleModelInfo::FLAG_HAS_PARACHUTE, LOC("PARAON"), CMDNAMES("vehparachute"), LOC("PARAON_H"));
				parachute->createChild<CommandVehicleParachuteModel>();
			};

			// Extras
			{
				auto extras = appearance->createChild<CommandListVehicleExtras>(LOC("XTRAS"));

				for (auto i = 0; i != 15; ++i)
				{
					extras->createChild<CommandVehicleExtra>(i);
				}
			}

			appearance->createChild<CommandScorched>();
			appearance->createChild<CommandTonk>();

			appearance->createChild<CommandCustomsShowNa>();

			appearance->createChild<CommandHeadlights>();
			appearance->createChild<CommandWindowTint>();
			appearance->createChild<CommandVehRoofLivery>();

			// Rest of the visual mods
			for(auto const& mod : VehicleMods::visual)
			{
				/*if (VehicleMods::isBooleanIncludeLights(mod))
				{
					appearance->createChild<CommandVehmodBool>(mod);
				}
				else*/
				{
					appearance->createChild<CommandVehmodInt>(mod);
				}
			}
		}

		this->createChild<CommandDivider>(LOC("TUNE_PRESETS"));

		this->createChild<CommandTune>();
		this->createChild<CommandPerf>();
		this->createChild<CommandPerfWithSpoiler>();
		this->createChild<CommandRandomTune>();
		this->createChild<CommandUntune>();
	}
}
