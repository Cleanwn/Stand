#pragma once

#include "VehicleCustomisationIo.hpp"

#include <unordered_map>

#include <soup/macros.hpp>

namespace Stand
{
	class ColonsVehicleCustomisationIo : public VehicleCustomisationIo
	{
	public:
		inline static const char* idx_names[] = {
			"Model",
			"Plate Style",
			"Plate Text",
			"Primary Colour",
			"Secondary Colour",
			"Pearl Colour",
			"Wheel Colour",
			"Wheel Type",
			"Spoiler",
			"Front Bumper",
			"Rear Bumper",
			"Sideskirt",
			"Exhaust",
			"Chassis",
			"Grille",
			"Hood",
			"Fender",
			"Right Fender",
			"Roof",
			"Engine",
			"Brakes",
			"Transmission",
			"Horns",
			"Suspension",
			"Armor",
			"Nitrous",
			"Turbo",
			"Subwoofer",
			"Tiresmoke",
			"Unk21",
			"Headlights",
			"Front Wheels",
			"Back Wheels",
			"Plate Holder",
			"Vanity Plates",
			"Trim",
			"Ornaments",
			"Dashboard",
			"Dial",
			"Seats",
			"Door Speaker",
			"Steering Wheel",
			"Shifter Leavers",
			"Plaques",
			"Speakers",
			"Trunk",
			"Hydraulics",
			"Engine Block",
			"Air Filter",
			"Struts",
			"Arch Cover",
			"Aerials",
			"Trim2",
			"Tank",
			"Left Door",
			"Right Door",
			"Livery",
			"Lightbar",
			"Tyre Smoke Colour",
			"Window Tint",
			"Custom Primary Colour",
			"Custom Secondary Colour",
			"Neon Colour",
			"Neon Front",
			"Neon Back",
			"Neon Left",
			"Neon Right",
			"Enveff Scale",
			"Interior Colour",
			"Roof Livery",
			"Dashboard Colour",
			"Tyres Mode",
			"Extra 1",
			"Extra 2",
			"Extra 3",
			"Extra 4",
			"Extra 5",
			"Extra 6",
			"Extra 7",
			"Extra 8",
			"Extra 9",
			"Extra 10",
			"Extra 11",
			"Extra 12",
			"Extra 13",
			"Extra 14",
			"Extra 15",
		};
		static_assert(COUNT(idx_names) == VehicleCustomisationIo::_NUM_PROPERIES);

		static bool handleSaveCommand(Click& click, std::wstring& args, VehicleCustomisationIo& veh_cio, AbstractEntity* veh = nullptr);

		std::unordered_map<std::string, std::string> data{};

		[[nodiscard]] Mixed read(int idx) final;
		void write(int idx, Mixed&& value) final;
	private:
		std::string writeInner(int idx, Mixed&& value) const;
	};
}
