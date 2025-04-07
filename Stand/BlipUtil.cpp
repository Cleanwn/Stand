#include "BlipUtil.hpp"

#include "atStringHash.hpp"
#include "natives.hpp"

namespace Stand
{
	void BlipUtil::remove(Blip blip)
	{
		MISC::SET_THIS_SCRIPT_CAN_REMOVE_BLIPS_CREATED_BY_ANY_SCRIPT(TRUE);
		HUD::SET_BLIP_DISPLAY(blip, 7);
		HUD::REMOVE_BLIP(&blip);
	}

	int BlipUtil::vehicleTypeToSprite(const int vehType)
	{
		switch (vehType)
		{
		case 1: return 421;
		case 2: return 424;
		case 3: return 423;
		case 4: return 422;
		case 5: return 426;
		case 6: return 427;
		case 7: return 533;
		case 8: return 534;
		case 9: return 530;
		case 10: return 531;
		case 11: return 528;
		case 12: return 529;
		case 13: return 532;
		case 14: return 512;
		case 15: return 558;
		case 16: return 559;
		case 17: return 560;
		case 18: return 561;
		case 19: return 562;
		case 20: return 563;
		case 21: return 572;
		case 22: return 573;
		case 23: return 574;
		case 24: return 575;
		case 25: return 576;
		case 26: return 577;
		case 27: return 578;
		case 28: return 579;
		case 29: return 580;
		case 30: return 581;
		case 31: return 582;
		case 32: return 583;
		case 33: return 584;
		case 34: return 585;
		case 35: return 595;
		case 36: return 596;
		case 37: return 597;
		case 38: return 598;
		case 39: return 599;
		case 40: return 600;
		case 41: return 601;
		case 42: return 602;
		case 43: return 603;
		case 44: return 589;
		case 45: return 460;
		case 48: return 631;
		case 49: return 632;
		case 50: return 633;
		case 51: return 634;
		case 52: return 635;
		case 53: return 636;
		case 54: return 637;
		case 56: return 640;
		case 55: return 639;
		case 57: return 658;
		case 58: return 659;
		case 59: return 660;
		case 60: return 661;
		case 61: return 662;
		case 62: return 663;
		case 63: return 664;
		case 64: return 665;
		case 65: return 666;
		case 66: return 667;
		case 67: return 668;
		case 68: return 669;
		case 46: return 612;
		case 47: return 613;
		case 69: return 746;
		case 70: return 753;
		case 71: return 758;
		case 72: return 348;
		case 73: return 749;
		case 74: return 757;
		case 75: return 754;
		case 76: return 745;
		case 77: return 755;
		case 78: return 759;
		case 79: return 747;
		case 80: return 748;
		case 81: return 750;
		case 82: return 760;
		case 83: return 824;
		case 84: return 825;
		case 85: return 823;
		case 86: return 820;
		case 87: return 821;
		case 88: return 818;
		}
		return 0;
	}

	hash_t BlipUtil::spriteToModel(const int spriteId)
	{
		switch (spriteId)
		{
		case 533: return ATSTRINGHASH("voltic2");
		case 530: return ATSTRINGHASH("ruiner2");
		case 531: return ATSTRINGHASH("dune4");
		//case 531: return ATSTRINGHASH("dune5");
		case 528: return ATSTRINGHASH("phantom2");
		case 534: return ATSTRINGHASH("technical");
		//case 534: return ATSTRINGHASH("technical2");
		//case 534: return ATSTRINGHASH("technical3");
		case 529: return ATSTRINGHASH("boxville5");
		case 532: return ATSTRINGHASH("wastelander");
		case 512: return ATSTRINGHASH("blazer5");
		case 558: return ATSTRINGHASH("apc");
		case 559: return ATSTRINGHASH("oppressor");
		case 560: return ATSTRINGHASH("halftrack");
		case 561: return ATSTRINGHASH("dune3");
		case 562: return ATSTRINGHASH("tampa3");
		case 563: return ATSTRINGHASH("trailersmall2");
		case 572: return ATSTRINGHASH("alphaz1");
		case 573: return ATSTRINGHASH("bombushka");
		case 574: return ATSTRINGHASH("havok");
		case 575: return ATSTRINGHASH("howard");
		case 576: return ATSTRINGHASH("hunter");
		case 577: return ATSTRINGHASH("microlight");
		case 578: return ATSTRINGHASH("mogul");
		case 579: return ATSTRINGHASH("molotok");
		case 580: return ATSTRINGHASH("nokota");
		case 581: return ATSTRINGHASH("pyro");
		case 582: return ATSTRINGHASH("rogue");
		case 583: return ATSTRINGHASH("starling");
		case 584: return ATSTRINGHASH("seabreeze");
		case 585: return ATSTRINGHASH("tula");
		case 595: return ATSTRINGHASH("stromberg");
		case 596: return ATSTRINGHASH("deluxo");
		case 597: return ATSTRINGHASH("thruster");
		case 598: return ATSTRINGHASH("khanjali");
		case 599: return ATSTRINGHASH("riot2");
		case 600: return ATSTRINGHASH("volatol");
		case 601: return ATSTRINGHASH("barrage");
		case 602: return ATSTRINGHASH("akula");
		case 603: return ATSTRINGHASH("chernobog");
		case 589: return ATSTRINGHASH("avenger");
		case 640: return ATSTRINGHASH("strikeforce");
		case 637: return ATSTRINGHASH("speedo4");
		case 636: return ATSTRINGHASH("mule4");
		case 635: return ATSTRINGHASH("pounder2");
		case 639: return ATSTRINGHASH("oppressor2");
		case 612: return ATSTRINGHASH("seasparrow");
		case 613: return ATSTRINGHASH("caracara");
		case 348: return ATSTRINGHASH("rrocket");
		case 760: return ATSTRINGHASH("kosatka");
		case 638: return ATSTRINGHASH("blimp3");
		case 631: return ATSTRINGHASH("pbus2");
		case 632: return ATSTRINGHASH("terbyte");
		case 633: return ATSTRINGHASH("menacer");
		case 634: return ATSTRINGHASH("scramjet");
		case 226: return ATSTRINGHASH("rrocket");
		case 427: return ATSTRINGHASH("dinghy4");
		case 753: return ATSTRINGHASH("seasparrow2");
		case 746: return ATSTRINGHASH("avisa");
		case 758: return ATSTRINGHASH("alkonost");
		case 749: return ATSTRINGHASH("verus");
		case 757: return ATSTRINGHASH("squaddie");
		case 754: return ATSTRINGHASH("dinghy5");
		case 745: return ATSTRINGHASH("winky");
		case 755: return ATSTRINGHASH("patrolboat");
		case 759: return ATSTRINGHASH("annihilator2");
		case 747: return ATSTRINGHASH("veto");
		case 748: return ATSTRINGHASH("veto2");
		case 750: return ATSTRINGHASH("vetir");
		case 421: return ATSTRINGHASH("rhino");
		case 308: return ATSTRINGHASH("submersible"); // || iParam0 == joaat("submersible2"))
		case 658: return ATSTRINGHASH("bruiser"); // || iParam0 == joaat("bruiser2")) || iParam0 == joaat("bruiser3"))
		case 659: return ATSTRINGHASH("brutus"); // || iParam0 == joaat("brutus2")) || iParam0 == joaat("brutus3"))
		case 660: return ATSTRINGHASH("cerberus"); // || iParam0 == joaat("cerberus2")) || iParam0 == joaat("cerberus3"))
		case 661: return ATSTRINGHASH("deathbike"); // || iParam0 == joaat("deathbike2")) || iParam0 == joaat("deathbike3"))
		case 662: return ATSTRINGHASH("dominator4"); // || iParam0 == joaat("dominator5")) || iParam0 == joaat("dominator6"))
		case 663: return ATSTRINGHASH("impaler2"); // || iParam0 == joaat("impaler3")) || iParam0 == joaat("impaler4"))
		case 664: return ATSTRINGHASH("imperator"); // || iParam0 == joaat("imperator2")) || iParam0 == joaat("imperator3"))
		case 665: return ATSTRINGHASH("issi4"); // || iParam0 == joaat("issi5")) || iParam0 == joaat("issi6"))
		case 666: return ATSTRINGHASH("monster3"); // || iParam0 == joaat("monster4")) || iParam0 == joaat("monster5"))
		case 667: return ATSTRINGHASH("scarab"); // || iParam0 == joaat("scarab2")) || iParam0 == joaat("scarab3"))
		case 668: return ATSTRINGHASH("slamvan4"); // || iParam0 == joaat("slamvan5")) || iParam0 == joaat("slamvan6"))
		case 669: return ATSTRINGHASH("zr380"); // || iParam0 == joaat("zr3802")) || iParam0 == joaat("zr3803"))
		case 824: return ATSTRINGHASH("champion");
		case 825: return ATSTRINGHASH("buffalo4");
		case 823: return ATSTRINGHASH("deity");
		case 820: return ATSTRINGHASH("jubilee");
		case 821: return ATSTRINGHASH("granger2");
		case 818: return ATSTRINGHASH("patriot3");
		}
		return 0;
	}

	Vector2Plus BlipUtil::getCoords(const Blip blip)
	{
		Vector2Plus pos;
		pos = HUD::GET_BLIP_COORDS(blip);
		if (pos.z == 0.0f)
		{
			pos.guessZ();
		}
		return pos;
	}

	Vector2Plus BlipUtil::closestBlipCoordOfType(const int spriteId)
	{
		return getCoords(HUD::GET_CLOSEST_BLIP_INFO_ID(spriteId));
	}
}
