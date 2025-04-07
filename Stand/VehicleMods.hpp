#pragma once

#include <cstdint>

#include "Label.hpp"

namespace Stand
{
	struct VehicleMods
	{
		static inline constexpr int all[] = {
			0, // Spoilers
			1, // Front Bumper
			2, // Rear Bumper
			3, // Side Skirt
			4, // Exhaust
			5, // Frame
			6, // Grille
			7, // Hood
			8, // Fender
			9, // Right Fender
			10, // Roof / Weapons
			11, // Engine
			12, // Brakes
			13, // Transmission
			14, // Horns
			15, // Suspension
			16, // Armor
			18, // Turbo
			22, // Xenon Headlights
			23, // Wheels
			24, // Back Wheels
			25, // Plate Holders
			26, // Vanity Plates
			27, // Trim Design
			28, // Ornaments
			29, // Dashboard
			30, // Dial Design
			32, // Door Speaker
			33, // Steering Wheel
			34, // Shift Lever
			35, // Plaques
			36, // Speakers
			37, // Trunk
			38, // Hydraulics
			39, // Engine Block
			40, // Boost / Air Filter
			41, // Struts
			42, // Arch Cover
			43, // Aerials
			44, // Trim
			45, // Tank
			48, // Livery
		};

		static inline constexpr int performance[] = {
			11, // Engine
			12, // Brakes
			13, // Transmission
			16, // Armour
			18, // Turbo Tuning
		};

		enum VehicleModTypes : int
		{
			// Standard
			/* SVMT_SPOILER */ spoiler,
			/* SVMT_BUMPER_F */ front_bumper,
			/* SVMT_BUMPER_R */ rear_bumper,
			/* SVMT_SKIRT */ sideskirt,
			/* SVMT_EXHAUST */ exhaust,
			/* SVMT_CHASSIS */ chassis, // Or roll cage
			/* SVMT_GRILL */ grille,
			/* SVMT_BONNET */ hood,
			/* SVMT_WING_L */ fender,
			/* SVMT_WING_R */ right_fender,
			/* SVMT_ROOF */ roof,
			/* SVMT_ENGINE */ engine,
			/* SVMT_BRAKES */ brakes,
			/* SVMT_GEARBOX */ transmission,
			/* SVMT_HORN */ horns,
			/* SVMT_SUSPENSION */ suspension,
			/* SVMT_ARMOUR */ armor,
			/* SVMT_NITROUS */ nitrous,
			/* SVMT_TURBO */ turbo,
			/* SVMT_SUBWOOFER */ subwoofer,
			/* SVMT_TYRE_SMOKE */ tiresmoke,
			/* SVMT_HYDRAULICS */ unk21, // Hydraulics?
			/* SVMT_XENON_LIGHTS */ xenon_lights,
			/* SVMT_WHEELS */ front_wheels,
			/* SVMT_WHEELS_REAR_OR_HYDRAULICS */ back_wheels, // Bikes only

			// Benny's
			/* SVMT_PLTHOLDER */ plate_holder,
			/* SVMT_PLTVANITY */ vanity_plates,
			/* SVMT_INTERIOR1 */ trim,
			/* SVMT_INTERIOR2 */ ornaments,
			/* SVMT_INTERIOR3 */ dashboard,
			/* SVMT_INTERIOR4 */ dial,
			/* SVMT_INTERIOR5 */ door_speaker,
			/* SVMT_SEATS */ seats,
			/* SVMT_STEERING */ steering_wheel,
			/* SVMT_KNOB */ shifter_leavers,
			/* SVMT_PLAQUE */ plaques,
			/* SVMT_ICE */ speakers,
			/* SVMT_TRUNK */ trunk,
			/* SVMT_HYDRO */ hydraulics,
			/* SVMT_ENGINEBAY1 */ engine_block,
			/* SVMT_ENGINEBAY2 */ air_filter,
			/* SVMT_ENGINEBAY3 */ struts,
			/* SVMT_CHASSIS2 */ arch_cover,
			/* SVMT_CHASSIS3 */ aerials,
			/* SVMT_CHASSIS4 */ trim2,
			/* SVMT_CHASSIS5 */ tank,
			/* SVMT_DOOR_L */ door_l,
			/* SVMT_DOOR_R */ door_r, // Right Door?
			/* SVMT_LIVERY */ livery,
			/* SVMT_LIGHTBAR */ lightbar, // Lightbar?

			_NUM_TYPES
		};

		static inline constexpr int visual[] =
		{
			horns,
			livery,
			sideskirt,
			suspension,
			exhaust,
			chassis,
			front_bumper,
			rear_bumper,
			hood,
			roof,
			fender,
			right_fender,
			grille,
			plate_holder,
			vanity_plates,
			trim,
			ornaments,
			dashboard,
			dial,
			seats,
			door_speaker,
			steering_wheel,
			shifter_leavers,
			plaques,
			speakers,
			trunk,
			hydraulics,
			engine_block,
			air_filter,
			struts,
			arch_cover,
			aerials,
			trim2,
			tank,
			door_l,
			door_r,
			lightbar,
		};

		[[nodiscard]] static constexpr bool isBooleanIncludeLights(const int modType)
		{
			return modType >= 17 && modType <= 22;
		}

		[[nodiscard]] static constexpr bool isBooleanExcludeLights(const int modType)
		{
			return modType >= 17 && modType <= 21;
		}

		[[nodiscard]] static constexpr bool hasVariation(const int modType)
		{
			switch (modType)
			{
			case front_wheels:
			case back_wheels:
				return true;
			}
			return false;
		}

		[[nodiscard]] static Label getName(int modType);
	};
}
