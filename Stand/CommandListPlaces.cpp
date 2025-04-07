#include "CommandListPlaces.hpp"

#include <array>

#include "PlaceWaypoint.hpp"
#include "PlaceObjective.hpp"
#include "PlaceSimple.hpp"
#include "PlaceSingle3d.hpp"
#include "PlaceClipboard.hpp"

#include "CommandAutoTpWp.hpp"
#include "CommandListSavedPlaces.hpp"
#include "CommandPlace.hpp"
#include "CommandWaypointPortal.hpp"

namespace Stand
{
	static const PlaceWaypoint waypoint{};
	static const PlaceObjective objective{};

	static const auto ammunation = PlaceSimple<rage::Vector2, 11>(LOC("AMMUNAT"), std::array<rage::Vector2, 11>{ rage::Vector2(1697.979126f, 3753.200439f), rage::Vector2(245.271103f, -45.812595f), rage::Vector2(844.124817f, -1025.570679f), rage::Vector2(-325.886658f, 6077.020020f), rage::Vector2(-664.217773f, -943.364624f), rage::Vector2(-1313.927856f, -390.968933f), rage::Vector2(-3165.230713f, 1082.855103f), rage::Vector2(2569.611572f, 302.575989f), rage::Vector2(-1111.251831f, 2688.476318f), rage::Vector2(811.869873f, -2149.101563f), rage::Vector2(17.680408f, -1114.287964f) });
	static const auto ammunation_with_range = PlaceSimple<rage::Vector2, 2>(LOC("SHTNGRNGE"), std::array<rage::Vector2, 2>{ rage::Vector2(811.869873f, -2149.101563f), rage::Vector2(17.680408f, -1114.287964f) });
	static const auto barber = PlaceSimple<rage::Vector2, 7>(LOC("BARBR"), std::array<rage::Vector2, 7>{ rage::Vector2(-821.9946f, -187.1776f), rage::Vector2(133.5702f, -1710.918f), rage::Vector2(-1287.0822f, -1116.5576f), rage::Vector2(1933.1191f, 3726.079f), rage::Vector2(1208.3335f, -470.917f), rage::Vector2(-30.7448f, -148.4921f), rage::Vector2(-280.8165f, 6231.7705f) });
	static const auto clothes = PlaceSimple<rage::Vector2, 14>(LOC("CLTHST"), std::array<rage::Vector2, 14>{ rage::Vector2(419.531f, -807.5787f), rage::Vector2(-818.6218f, -1077.533f), rage::Vector2(80.665f, -1391.6694f), rage::Vector2(1687.8812f, 4820.55f), rage::Vector2(-1094.0487f, 2704.1707f), rage::Vector2(1197.9722f, 2704.2205f), rage::Vector2(-0.2361f, 6516.0454f), rage::Vector2(-715.3598f, -155.7742f), rage::Vector2(-158.2199f, -304.9663f), rage::Vector2(-1455.0045f, -233.1862f), rage::Vector2(-1199.8092f, -776.6886f), rage::Vector2(618.1857f, 2752.5667f), rage::Vector2(126.6853f, -212.5027f), rage::Vector2(-3168.9663f, 1055.2869f) });
	static const auto lsc = PlaceSimple<v3, 4>(LOC("LSC"), std::array<v3, 4>{ v3(-1147.202759f, -1992.451782f, 12.653099f), v3(724.479248f, -1089.010254f, 21.648642f), v3(-354.587860f, -135.499176f, 38.479984f), v3(1174.730347f, 2644.499023f, 37.240135f) });
	static const auto tattoo = PlaceSimple<rage::Vector2, 6>(LOC("TATTOO"), std::array<rage::Vector2, 6>{ rage::Vector2(321.6098f, 179.4165f), rage::Vector2(1861.6853f, 3750.0798f), rage::Vector2(-290.1603f, 6199.0947f), rage::Vector2(-1153.9481f, -1425.0186f), rage::Vector2(1322.4547f, -1651.1252f), rage::Vector2(-3169.4204f, 1074.7272f) });

	static const auto lsia = PlaceSingle3d(LOC("LSIA"), v3(-1087.7434, -3015.614, 13.825307));
	static const auto mazehelipad = PlaceSingle3d(LOC("MAZEHELIPAD"), v3(-75.2188f, -818.582f, 326.175f));
	static const auto fortzancudo = PlaceSingle3d(LOC("FRTZNC"), v3(-2285.9294f, 3124.115f, 32.81487f));
	static const auto mtchiliad = PlaceSingle3d(LOC("MTCHIL"), v3(501.77032f, 5595.622f, 795.77277f));
	static const auto cayo_perico = PlaceSingle3d(LOC("CAYO"), v3(4906.2554f, -4912.7646f, 3.3630776f));
	static const auto casino = PlaceSingle3d(LOC("CSNO"), v3(922.816223f, 47.206078f, 81.106331f));

	static const auto highplace = PlaceSingle3d(LOC("HIGHPLACE"), v3(-75.2188f, -818.582f, 2500.0f));

	static const PlaceClipboard clipboard{};

	void CommandListPlaces::populate(bool for_teleport)
	{
		if (for_teleport)
		{
			this->createChild<CommandPlace>((Place*)&waypoint, CMDNAMES("tpwp", "tpwaypoint", "teleportwaypoint"));
		}
		this->createChild<CommandPlace>((Place*)&objective, decorateCommandNames({ CMDNAME("objective") }));
		this->createChildWithBuilder<CommandListSavedPlaces>();
		{
			auto* const stores = this->createChild<CommandList>(LOC("STORS"));

			stores->createChild<CommandPlace>((Place*)&ammunation, decorateCommandNames({ CMDNAME("ammunation"), CMDNAME("amunation") }));
			stores->createChild<CommandPlace>((Place*)&ammunation_with_range, decorateCommandNames({ CMDNAME("shootingrange") }));
			stores->createChild<CommandPlace>((Place*)&barber, decorateCommandNames({ CMDNAME("barbershop") }));
			stores->createChild<CommandPlace>((Place*)&clothes, decorateCommandNames({ CMDNAME("clothesstore") }));
			stores->createChild<CommandPlace>((Place*)&lsc, decorateCommandNames({ CMDNAME("lsc") }));
			stores->createChild<CommandPlace>((Place*)&tattoo, decorateCommandNames({ CMDNAME("tattooparlor") }));
		}
		{
			auto* const landmarks = this->createChild<CommandList>(LOC("LNDMRKS"));

			landmarks->createChild<CommandPlace>((Place*)&lsia, decorateCommandNames({ CMDNAME("lsia") }));
			landmarks->createChild<CommandPlace>((Place*)&mazehelipad, decorateCommandNames({ CMDNAME("mazehelipad") }));
			landmarks->createChild<CommandPlace>((Place*)&fortzancudo, decorateCommandNames({ CMDNAME("fortzancudo") }));
			landmarks->createChild<CommandPlace>((Place*)&mtchiliad, decorateCommandNames({ CMDNAME("mtchiliad") }));
			landmarks->createChild<CommandPlace>((Place*)&cayo_perico, decorateCommandNames({ CMDNAME("cayoperico") }), LOC("TP_CAYO"));
			landmarks->createChild<CommandPlace>((Place*)&casino, decorateCommandNames({ CMDNAME("casino") }));
		}
		if (for_teleport)
		{
			this->createChild<CommandPlace>((Place*)&highplace, CMDNAMES("tphigh", "tpveryhigh", "teleporthigh", "teleportveryhigh"));
			this->createChild<CommandPlace>((Place*)&clipboard, CMDNAMES("pastepos", "pastecoords", "tpclipboard", "teleportclipboard"), LOC("CBPOS_H"));
			this->createChild<CommandAutoTpWp>();
			this->createChild<CommandWaypointPortal>();
		}
		else
		{
			this->createChild<CommandPlace>((Place*)&clipboard, decorateCommandNames({ CMDNAME("clipboard") }), LOC("CBPOS_H"));
		}
	}
}
