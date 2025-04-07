#pragma once

#include "CommandListAction.hpp"

#include "AbstractEntity.hpp"
#include "CommandListActionItem.hpp"
#include "CommandListSelect.hpp"
#include "CommandSliderFloat.hpp"
#include "eBlip.hpp"
#include "get_current_time_millis.hpp"
#include "Vector2Plus.hpp"

namespace Stand
{
	class CommandAutoDriveAction : public CommandListAction
	{
	private:
		enum : uint16_t
		{
			NOWHERE,
			WAYPOINT,
			OBJECTIVE,
			CASINO = eBlip::radar_casino,
			AMMUNATION = eBlip::radar_gun_shop,
			CLOTHES = eBlip::radar_clothes_store,
			TATS = eBlip::radar_tattoo,
			SALON = eBlip::radar_barber,
			LSC = eBlip::radar_car_mod_shop,
			MOVIES = eBlip::radar_cinema,
			STORE,
			CLUBHOUSE = eBlip::radar_biker_clubhouse,
			FACILITY = eBlip::radar_nhp_base,
			OFFICE = eBlip::radar_office,
			AGENCY = eBlip::radar_agency,
			HANGAR = eBlip::radar_sm_hangar,
		};

		struct PredefinedParkingLocation
		{
			uint16_t id;
			v3 area;
		};

		// "Proximity" destinations are often really low quality. These positions are tailored to the disapointing pathfinding algorithms.
		static constexpr PredefinedParkingLocation predefined_areas[] = {
			PredefinedParkingLocation(LSC, { 714.51776f, -1083.2561f, 21.854647f }), // Near Police Station, La Mesa
			PredefinedParkingLocation(LSC, { 123.02218f, 6610.1196f, 31.664734f }), // Paleto
			PredefinedParkingLocation(LSC, { -1132.8667f, -1987.2146f, 12.9606085f }), // Airport
			PredefinedParkingLocation(LSC, { -375.31482f, -124.690834f, 38.450546f }), // Downtown
			PredefinedParkingLocation(LSC, { 1185.6864f, 2657.0488f, 37.613174f }), // Sandy
			PredefinedParkingLocation(AMMUNATION, { 842.3504f, -1012.8778f, 27.587177f }), // Near Police Station, La Mesa
			PredefinedParkingLocation(AMMUNATION, { 234.66951f, -43.042564f, 69.46249f }), // Hawick, A few blocks underneath movies
			PredefinedParkingLocation(AMMUNATION, { -317.86472f, 6070.304f, 30.991272f }), // Paleto
			PredefinedParkingLocation(AMMUNATION, { -3158.4033f, 1079.3243f, 20.48468f }), // Chumash
			PredefinedParkingLocation(AMMUNATION, { 1706.5652f, 3748.798f, 33.746174f }), // Sandy
			PredefinedParkingLocation(AMMUNATION, { 2571.19f, 312.44772f, 108.25545f }), // Palomino Freeway
			PredefinedParkingLocation(AMMUNATION, { -1107.5918f, 2685.5735f, 18.78944f }), // Zancudo
			PredefinedParkingLocation(AMMUNATION, { -1322.1334f, -389.24292f, 36.292675f }), // Morningwood, Near Golf
			PredefinedParkingLocation(AMMUNATION, { 16.524305f, -1125.8948f, 28.55648f }), // Strawberry
			PredefinedParkingLocation(AMMUNATION, { 813.6088f, -2128.7588f, 29.091505f }), // Docks
			PredefinedParkingLocation(CLOTHES, { 136.37126f, -200.91557f, 54.137936f }), // Hawick
			PredefinedParkingLocation(CLOTHES, { -147.6095f, -305.91977f, 38.59956f }), // Posonbys, Burton, Downtown near LSC
			PredefinedParkingLocation(CLOTHES, { -713.37384f, -173.11232f, 36.626865f }), // Rockford Hills
			PredefinedParkingLocation(CLOTHES, { -1457.8241f, -227.80531f, 48.941963f }), // Morningwood, Near Golf
			PredefinedParkingLocation(CLOTHES, { -1209.3428f, -786.56244f, 16.682693f }), // Del Perro, Near Police Station
			PredefinedParkingLocation(CLOTHES, { -814.9048f, -1084.2781f, 10.819783f }), // Binco, Near Vespucci Canals
			PredefinedParkingLocation(CLOTHES, { 87.32159f, -1391.8502f, 29.00283f }), // Strawberry
			PredefinedParkingLocation(CLOTHES, { 411.44565f, -806.32434f, 28.959385f }), // Textile City, Near Bus Station
			PredefinedParkingLocation(CLOTHES, { 617.31f, 2738.0125f, 41.759796f }), // Suburban in Sandy Shores, near end of Route 68
			PredefinedParkingLocation(CLOTHES, { 1184.732f, 2695.8882f, 37.749767f }), // The opposite Suburban in Sandy Shores
			PredefinedParkingLocation(CLOTHES, { -3.8964117f, 6520.841f, 31.087736f }), // Paleto
			PredefinedParkingLocation(CLOTHES, { -3163.2078f, 1066.0485f, 20.469896f }), // Chumash
			PredefinedParkingLocation(CLOTHES, { -1093.0728f, 2697.2905f, 19.695257f }), // Zancudo
			PredefinedParkingLocation(TATS, { 324.5403f, 165.07625f, 103.26164f }), // Downtown Vinewood
			PredefinedParkingLocation(TATS, { -1160.3214f, -1415.7888f, 4.5720935f }), // Vespucci
			PredefinedParkingLocation(TATS, { -3162.754f, 1068.8292f, 20.471384f }), // Chumash
			PredefinedParkingLocation(TATS, { 1313.4674f, -1638.5048f, 51.881187f }), // Oil Fields
			PredefinedParkingLocation(TATS, { -286.26758f, 6204.5337f, 31.160702f }), // Paleto
			PredefinedParkingLocation(TATS, { 1858.7087f, 3736.912f, 32.89672f }), // Sandy, Tuned
			PredefinedParkingLocation(CASINO, { 920.83527f, 49.71116f, 80.691635f }), // There's only one, I needn't say.
			PredefinedParkingLocation(SALON, { 1197.4283f, -472.0699f, 65.81584f }), // Near Casino
			PredefinedParkingLocation(SALON, { 1937.9331f, 3718.1128f, 32.129326f }), // Sandy
			PredefinedParkingLocation(SALON, { -1296.6162f, -1116.6342f, 6.26645f }), // Vespucci
			PredefinedParkingLocation(SALON, { -30.324747f, -138.45311f, 56.731667f }), // Hawick
			PredefinedParkingLocation(SALON, { 128.99533f, -1716.0093f, 28.863811f }), // Strawberry
			PredefinedParkingLocation(SALON, { -829.5641f, -192.23682f, 37.21811f }), // Rockford Hills
			PredefinedParkingLocation(SALON, { -287.742f, 6237.9966f, 31.099491f }), // Paleto
			PredefinedParkingLocation(MOVIES, { 294.20438f, 176.2208f, 103.65391f }), // Vinewood
			PredefinedParkingLocation(MOVIES, { -1425.1964f, -193.37956f, 46.753994f }), // Morningwood
			PredefinedParkingLocation(MOVIES, { -1425.1964f, -193.37956f, 46.753994f }), // Morningwood
			PredefinedParkingLocation(STORE, { -69.33865f, -1750.3923f, 29.0265f }), // Strawberry
			PredefinedParkingLocation(STORE, { 375.81036f, 315.94232f, 102.81324f }), // Vinewood
			PredefinedParkingLocation(STORE, { -1230.5641f, -896.85583f, 11.667201f }), // Vespucci
			PredefinedParkingLocation(STORE, { 1694.0984f, 4941.4727f, 41.77365f }), // Grapeseed
			PredefinedParkingLocation(STORE, { -712.6548f, -922.97687f, 18.54737f }), // Little Seoul
			// PredefinedParkingLocation(STORE, { 1159.9679f, -334.36313f, 68.184586f }), // Mirror Park. This specific position seems to really creep out the AI. They cannot pathfind towards it at all if they are near certain parts of the Casino.
			PredefinedParkingLocation(STORE, { -1817.2085f, 781.7434f, 137.12236f }), // Richman
			PredefinedParkingLocation(STORE, { -3045.0762f, 600.3076f, 6.9654055f }), // Chumash, Middle
			PredefinedParkingLocation(STORE, { -3241.6755f, 990.84f, 11.986698f }), // Chumash, Top
			PredefinedParkingLocation(STORE, { 541.46826f, 2679.9458f, 41.782463f }), // Sandy, Route 68
			PredefinedParkingLocation(STORE, { 2568.4253f, 386.14166f, 107.99329f }), // Palomino Freeway
			PredefinedParkingLocation(STORE, { 2688.1687f, 3281.1384f, 54.773693f }), // Sandy Freeway
			PredefinedParkingLocation(STORE, { 1726.1534f, 6400.6304f, 33.986847f }), // Northern Paleto Highway
			PredefinedParkingLocation(STORE, { 1969.2423f, 3735.3145f, 31.832655f }), // Sandy
			PredefinedParkingLocation(STORE, { 23.557364f, -1356.7196f, 28.74953f }), // Strip Club
			PredefinedParkingLocation(STORE, { 1173.875f, 2693.2556f, 37.41823f }), // Sandy, Route 68, Near LSC
			PredefinedParkingLocation(STORE, { -2981.1665f, 392.4887f, 14.404157f }), // Chumash, Lower
			PredefinedParkingLocation(STORE, { 1148.3352f, -981.3548f, 45.63166f }), // Murrieta Heights
			PredefinedParkingLocation(STORE, { -1508.429f, -381.09238f, 40.64172f }), // Morningwood
			PredefinedParkingLocation(CLUBHOUSE, { 238.52287f, -1807.2628f, 27.457933f }), // Strawberry
			PredefinedParkingLocation(CLUBHOUSE, { -1143.8768f, -1584.4332f, 4.430591f }), // Vespucci
			PredefinedParkingLocation(CLUBHOUSE, { 940.2551f, -1451.3026f, 31.063656f }), // La Mesa
			PredefinedParkingLocation(CLUBHOUSE, { 31.973688f, -1026.7087f, 29.464502f }), // Pillbox Hill
			PredefinedParkingLocation(CLUBHOUSE, { -1462.5527f, -922.4953f, 10.045345f }), // Vespucci but more near the Police Station
			PredefinedParkingLocation(CLUBHOUSE, { -25.59567f, -198.31888f, 52.358723f }), // Downtown
			PredefinedParkingLocation(CLUBHOUSE, { 186.12376f, 300.6886f, 105.449165f }), // Vinewood
			PredefinedParkingLocation(CLUBHOUSE, { 43.284985f, 2781.908f, 57.87817f }), // Zancudo / Route 68
			PredefinedParkingLocation(CLUBHOUSE, { 1725.2417f, 3718.4246f, 34.13504f }), // Sandy
			PredefinedParkingLocation(CLUBHOUSE, { 2484.2705f, 4118.1187f, 38.06422f }), // Grapeseed
			PredefinedParkingLocation(CLUBHOUSE, { -28.270483f, 6410.762f, 30.980844f }), // Paleto, Closer to Beeker's
			PredefinedParkingLocation(CLUBHOUSE, { -358.9967f, 6075.683f, 31.00442f }), // Paleto, Closer to Ammunation
			PredefinedParkingLocation(FACILITY, { 1861.4219f, 263.92984f, 163.2653f }), // Dam
			PredefinedParkingLocation(FACILITY, { 2089.6545f, 1732.7697f, 103.1905f }), // Windfarm
			PredefinedParkingLocation(FACILITY, { -2214.9917f, 2313.7542f, 32.650734f }), // Zancudo
			PredefinedParkingLocation(FACILITY, { 40.112694f, 2625.29f, 84.20426f }), // Route 58
			PredefinedParkingLocation(FACILITY, { 36.738426f, 3307.9036f, 38.506927f }), // Sandy, near Biker Place
			PredefinedParkingLocation(FACILITY, { 1281.6306f, 2814.8765f, 46.121983f }), // Sandy, near Airfield
			PredefinedParkingLocation(FACILITY, { 2729.9104f, 3886.179f, 43.450138f }), // Sandy, Senora Hwy
			PredefinedParkingLocation(FACILITY, { 3332.9924f, 5471.2773f, 19.649874f }), // Grapeseed
			PredefinedParkingLocation(FACILITY, { 68.108284f, 6790.1177f, 19.513039f }), // Paleto
			PredefinedParkingLocation(OFFICE, { -1376.7617f, -538.5982f, 29.590904f }), // Morningwood
			PredefinedParkingLocation(OFFICE, { -1566.4264f, -527.0458f, 35.047073f }), // Morningwood, but closer to Vespucci
			PredefinedParkingLocation(OFFICE, { -53.633976f, -781.6521f, 43.723034f }), // Maze Bank
			PredefinedParkingLocation(OFFICE, { -106.44698f, -610.2449f, 35.572304f }), // Pillbox Hill, Near Maze Bank
			PredefinedParkingLocation(AGENCY, { -1047.7487f, -389.01276f, 37.109055f }), // Rockford Hills
			PredefinedParkingLocation(AGENCY, { -1045.7153f, -749.13416f, 18.560139f }), // Vespucci Canals
			PredefinedParkingLocation(AGENCY, { -589.1103f, -668.93616f, 31.760002f }), // Little Seoul
			PredefinedParkingLocation(AGENCY, { 374.19016f, -97.651276f, 66.19554f }), // Hawick
			PredefinedParkingLocation(HANGAR, { -1137.6631f, -3388.541f, 13.454379f }), // LSIA, Left
			PredefinedParkingLocation(HANGAR, { -1382.6698f, -3247.5823f, 13.459947f }), // LSIA, Right
			PredefinedParkingLocation(HANGAR, { -1886.4453f, 3093.5142f, 32.32572f }), // Zancudo, Right
			PredefinedParkingLocation(HANGAR, { -2026.5764f, 3146.0144f, 32.324833f }), // Zancudo, Middle
			PredefinedParkingLocation(HANGAR, { -2477.7375f, 3262.346f, 32.34297f }), // Zancudo, Left
		};

		[[nodiscard]] Vector2Plus getDestinationCoord(Click& click, uint16_t destination);

		[[nodiscard]] float getCruiseSpeed() const;

		[[nodiscard]] bool isPathPredefined(const v3& destination);
		[[nodiscard]] bool isSingular(uint16_t destination) const;
		[[nodiscard]] bool isAttemptingToMove() const;

		bool should_die = false;
		bool handler_running = false;
		time_t last_speed_update = 0;
		AbstractEntity vehicle = AbstractEntity::invalid();

	public:
		Vector2Plus destination_coords;
		CommandToggle* beacon = nullptr;
		CommandToggle* auto_kill = nullptr;
		CommandToggle* noclipping = nullptr;
		CommandListSelect* mode = nullptr;
		CommandSliderFloat* speed = nullptr;

		static constexpr float default_cruise_speed = 12.0f;

		explicit CommandAutoDriveAction(CommandList* const parent);

		void onItemClick(Click& click, CommandListActionItem* item) final;
		void applyBehaviour(Vector2Plus&& destination);
		void doTickHandler();
		void requestDeath();

	private:
		void kill();
	};
}