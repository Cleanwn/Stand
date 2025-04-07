#include "AptHelper.hpp"

#include "Util.hpp"

namespace Stand
{
	v3 AptHelper::getPosFromId(const uint8_t pos_id)
	{
		// 1.52 freemode func_1506
		switch (pos_id)
		{
		default:
		case 1:
			return { -773.4775f, 310.6321f, 84.6981f };

		case 2:
			return { -252.5713f, -949.9199f, 30.221f };

		case 3:
			return { -1443.094f, -544.7684f, 33.7424f };

		case 4:
			return { -913.85f, -455.1392f, 38.5999f };

		case 5:
			return { -47.3145f, -585.9766f, 36.9593f };

		case 6:
			return { -932.7474f, -383.9246f, 37.9613f };

		case 7:
			return { -619.1315f, 37.8841f, 42.5883f };

		case 8:
			return { 284.9614f, -159.9891f, 63.6221f };

		case 9:
			return { 2.8889f, 35.7762f, 70.5349f };

		case 10:
			return { 9.74f, 84.6906f, 77.3975f };

		case 11:
			return { -512.1465f, 111.2223f, 62.351f };

		case 12:
			return { -197.3405f, 88.1053f, 68.7422f };

		case 13:
			return { -628.3212f, 165.8364f, 60.1651f };

		case 14:
			return { -973.3757f, -1429.425f, 6.6791f };

		case 15:
			return { -829.1362f, -855.0104f, 18.6297f };

		case 16:
			return { -757.5739f, -755.5591f, 25.5697f };

		case 17:
			return { -45.1289f, -57.0925f, 62.2531f };

		case 18:
			return { -206.7293f, 184.142f, 79.3279f };

		case 19:
			return { -811.7045f, -984.1961f, 13.1538f };

		case 20:
			return { -664.0032f, -853.6744f, 23.4325f };

		case 21:
			return { -1534.025f, -324.5247f, 46.5237f };

		case 22:
			return { -1561.381f, -412.1974f, 41.389f };

		case 23:
			return { -1608.851f, -429.184f, 39.439f };

		case 24:
			return { 964.3583f, -1034.199f, 39.8303f };

		case 25:
			return { 894.2868f, -885.5679f, 26.1212f };

		case 26:
			return { 821.1741f, -924.1658f, 25.1998f };

		case 27:
			return { 759.7933f, -759.8209f, 25.759f };

		case 28:
			return { 844.7289f, -1164.1f, 24.2681f };

		case 29:
			return { 526.2521f, -1604.613f, 28.2625f };

		case 30:
			return { 572.0462f, -1570.736f, 27.4904f };

		case 31:
			return { 722.2852f, -1190.617f, 23.282f };

		case 32:
			return { 497.6212f, -1494.084f, 28.2893f };

		case 33:
			return { 480.3127f, -1549.937f, 28.2828f };

		case 34:
			return { -68.702f, 6426.148f, 30.439f };

		case 35:
			return { -247.4374f, 6240.294f, 30.4892f };

		case 36:
			return { 2554.165f, 4668.059f, 33.0233f };

		case 37:
			return { 2461.22f, 1589.255f, 32.0443f };

		case 38:
			return { -2203.335f, 4244.427f, 47.3305f };

		case 39:
			return { 218.0665f, 2601.817f, 44.7668f };

		case 40:
			return { 186.1719f, 2786.343f, 45.0144f };

		case 41:
			return { 642.0746f, 2791.729f, 40.9795f };

		case 42:
			return { -1130.938f, 2701.133f, 17.8004f };

		case 43:
			return { -10.944f, -1646.76f, 28.3125f };

		case 44:
			return { 1024.263f, -2398.404f, 29.1261f };

		case 45:
			return { 870.8577f, -2232.323f, 29.5508f };

		case 46:
			return { -663.8541f, -2380.389f, 12.9446f };

		case 47:
			return { -1088.616f, -2235.098f, 12.2182f };

		case 48:
			return { -342.5126f, -1468.675f, 29.6107f };

		case 49:
			return { -1241.54f, -259.8841f, 37.9445f };

		case 50:
			return { 899.8448f, -147.528f, 75.5674f };

		case 51:
			return { -1405.411f, 526.8572f, 122.8361f };

		case 52:
			return { 1341.552f, -1578.037f, 53.4443f };

		case 53:
			return { -105.7029f, 6528.539f, 29.1719f };

		case 54:
			return { -302.3985f, 6327.434f, 31.8918f };

		case 55:
			return { -15.258f, 6557.378f, 32.2454f };

		case 56:
			return { 1899.673f, 3773.178f, 31.8829f };

		case 57:
			return { 1662.121f, 4776.317f, 41.0075f };

		case 58:
			return { -178.2278f, 490.886f, 134.0466f };

		case 59:
			return { 339.5743f, 439.7083f, 145.5896f };

		case 60:
			return { -764.6163f, 618.3144f, 137.5536f };

		case 61:
			return { -679.5461f, 592.5162f, 139.693f };

		case 62:
			return { 124.4571f, 551.8877f, 181.658f };

		case 63:
			return { -563.7349f, 689.099f, 151.6596f };

		case 64:
			return { -743.0927f, 590.0371f, 140.9221f };

		case 65:
			return { -861.252f, 684.8923f, 146.2643f };

		case 66:
			return { -1292.456f, 440.9454f, 93.7572f };

		case 67:
			return { 369.5891f, 417.4813f, 136.8344f };

		case 68:
			return { -1581.501f, -558.2578f, 33.9528f };

		case 69:
			return { -1379.546f, -499.1783f, 32.1574f };

		case 70:
			return { -117.5296f, -605.7157f, 35.2857f };

		case 71:
			return { -67.0943f, -802.4415f, 43.2273f };

		case 72:
			return { 254.1892f, -1809.183f, 26.1805f };

		case 73:
			return { -1472.278f, -920.0677f, 8.9683f };

		case 74:
			return { 38.9478f, -1026.629f, 28.6354f };

		case 75:
			return { 46.903f, 2789.825f, 57.1124f };

		case 76:
			return { -342.3246f, 6065.316f, 30.4895f };

		case 77:
			return { 1737.878f, 3709.088f, 33.1348f };

		case 78:
			return { 939.7161f, -1459.204f, 30.467f };

		case 79:
			return { 189.7624f, 309.7488f, 104.4714f };

		case 80:
			return { -21.9593f, -191.3618f, 51.5599f };

		case 81:
			return { 2472.22f, 4110.493f, 36.9629f };

		case 82:
			return { -39.3286f, 6420.603f, 30.7017f };

		case 83:
			return { -1134.762f, -1568.848f, 3.4077f };
		}
	}

	uint8_t AptHelper::getNearestPosId(const v3& target)
	{
		uint8_t nearest_pos_id = 1;
		float nearest_dist = FLT_MAX;
		for (uint8_t pos_id = 1; pos_id <= max_pos_id; pos_id++)
		{
			const float dist = getPosFromId(pos_id).distance(target);
			if (dist < nearest_dist)
			{
				nearest_pos_id = pos_id;
				nearest_dist = dist;
			}
		}
		return nearest_pos_id;
	}

	v3 AptHelper::getNearestPos(const v3& target)
	{
		v3 nearest_pos = {};
		float nearest_dist = FLT_MAX;
		for (uint8_t pos_id = 1; pos_id <= max_pos_id; pos_id++)
		{
			const auto pos = getPosFromId(pos_id);
			const float dist = pos.distance(target);
			if (dist < nearest_dist)
			{
				nearest_pos = pos;
				nearest_dist = dist;
			}
		}
		return nearest_pos;
	}

	uint8_t AptHelper::propertyIdToPosId(const uint8_t property_id)
	{
		// 1.52 freemode func_1507
		switch (property_id)
		{
		default:
		case 1:
		case 2:
		case 3:
		case 4:
		case 61:
		case 83:
		case 84:
		case 85:
			return 1;
			break;

		case 5:
		case 6:
			return 2;
			break;

		case 7:
		case 34:
		case 62:
			return 3;
			break;

		case 35:
		case 36:
		case 37:
			return 4;
			break;

		case 38:
		case 39:
		case 65:
			return 5;
			break;

		case 40:
		case 41:
		case 63:
			return 6;
			break;

		case 42:
		case 43:
		case 64:
			return 7;
			break;

		case 8:
			return 8;
			break;

		case 9:
			return 9;
			break;

		case 10:
			return 10;
			break;

		case 11:
			return 11;
			break;

		case 12:
			return 12;
			break;

		case 13:
			return 13;
			break;

		case 14:
			return 14;
			break;

		case 15:
			return 15;
			break;

		case 16:
			return 16;
			break;

		case 17:
			return 17;
			break;

		case 18:
			return 18;
			break;

		case 19:
			return 19;
			break;

		case 20:
			return 20;
			break;

		case 21:
			return 21;
			break;

		case 22:
			return 22;
			break;

		case 23:
			return 23;
			break;

		case 24:
			return 24;
			break;

		case 25:
			return 25;
			break;

		case 26:
			return 26;
			break;

		case 27:
			return 27;
			break;

		case 28:
			return 28;
			break;

		case 29:
			return 29;
			break;

		case 30:
			return 30;
			break;

		case 31:
			return 31;
			break;

		case 32:
			return 32;
			break;

		case 33:
			return 33;
			break;

		case 44:
			return 34;
			break;

		case 45:
			return 35;
			break;

		case 46:
			return 36;
			break;

		case 47:
			return 37;
			break;

		case 48:
			return 38;
			break;

		case 49:
			return 39;
			break;

		case 50:
			return 40;
			break;

		case 51:
			return 41;
			break;

		case 52:
			return 42;
			break;

		case 53:
			return 43;
			break;

		case 54:
			return 44;
			break;

		case 55:
			return 45;
			break;

		case 56:
			return 46;
			break;

		case 57:
			return 47;
			break;

		case 58:
			return 48;
			break;

		case 59:
			return 49;
			break;

		case 60:
			return 50;
			break;

		case 66:
			return 51;
			break;

		case 67:
			return 52;
			break;

		case 68:
			return 53;
			break;

		case 69:
			return 54;
			break;

		case 70:
			return 55;
			break;

		case 71:
			return 56;
			break;

		case 72:
			return 57;
			break;

		case 73:
			return 58;
			break;

		case 74:
			return 59;
			break;

		case 75:
			return 60;
			break;

		case 76:
			return 61;
			break;

		case 77:
			return 62;
			break;

		case 78:
			return 63;
			break;

		case 79:
			return 64;
			break;

		case 80:
			return 65;
			break;

		case 81:
			return 66;
			break;

		case 82:
			return 67;
			break;

		case 87:
		case 103:
		case 104:
		case 105:
			return 68;
			break;

		case 88:
		case 106:
		case 107:
		case 108:
			return 69;
			break;

		case 89:
		case 109:
		case 110:
		case 111:
			return 70;
			break;

		case 90:
		case 112:
		case 113:
		case 114:
			return 71;
			break;

		case 91:
			return 72;
			break;

		case 92:
			return 73;
			break;

		case 93:
			return 74;
			break;

		case 94:
			return 75;
			break;

		case 95:
			return 76;
			break;

		case 96:
			return 77;
			break;

		case 97:
			return 78;
			break;

		case 98:
			return 79;
			break;

		case 99:
			return 80;
			break;

		case 100:
			return 81;
			break;

		case 101:
			return 82;
			break;

		case 102:
			return 83;
			break;
		}
	}

	uint8_t AptHelper::posIdToPropertyId(const uint8_t pos_id)
	{
		for (uint8_t property_id = 1; property_id <= max_property_id; property_id++)
		{
			if (propertyIdToPosId(property_id) == pos_id)
			{
				return property_id;
			}
		}
		return 1;
	}

	std::string AptHelper::propertyIdToName(const uint8_t property_id)
	{
		switch (property_id)
		{
		default:
		case 1:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_1"));

		case 2:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_2"));

		case 3:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_3"));

		case 4:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_4"));

		case 5:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_5"));

		case 6:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_6"));

		case 7:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_7"));

		case 8:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_8"));

		case 9:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_9"));

		case 10:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_10"));

		case 11:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_11"));

		case 12:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_12"));

		case 13:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_13"));

		case 14:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_14"));

		case 15:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_15"));

		case 16:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_16"));

		case 17:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_17"));

		case 18:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_18"));

		case 19:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_19"));

		case 20:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_20"));

		case 21:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_21"));

		case 22:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_22"));

		case 23:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_23"));

		case 24:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_24"));

		case 25:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_25"));

		case 26:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_26"));

		case 27:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_27"));

		case 28:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_28"));

		case 29:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_29"));

		case 30:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_30"));

		case 31:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_31"));

		case 32:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_32"));

		case 33:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_33"));

		case 34:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_34"));

		case 35:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_35"));

		case 36:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_36"));

		case 37:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_37"));

		case 38:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_38"));

		case 39:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_39"));

		case 40:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_40"));

		case 41:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_41"));

		case 42:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_42"));

		case 43:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_43"));

		case 44:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_44"));

		case 45:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_45"));

		case 46:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_46"));

		case 47:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_48"));

		case 48:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_49"));

		case 49:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_50"));

		case 50:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_51"));

		case 51:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_52"));

		case 52:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_57"));

		case 53:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_59"));

		case 54:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_60"));

		case 55:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_61"));

		case 56:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_62"));

		case 57:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_63"));

		case 58:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_64"));

		case 59:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_65"));

		case 60:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_66"));

		case 61:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_67"));

		case 62:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_68"));

		case 63:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_69"));

		case 64:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_70"));

		case 65:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_71"));

		case 66:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_72"));

		case 67:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_73"));

		case 68:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_74"));

		case 69:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_75"));

		case 70:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_76"));

		case 71:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_77"));

		case 72:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_78"));

		case 83:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_79"));

		case 84:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_80"));

		case 85:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_81"));

		case 73:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_83"));

		case 74:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_84"));

		case 75:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_85"));

		case 76:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_86"));

		case 77:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_87"));

		case 78:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_89"));

		case 79:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_90"));

		case 80:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_92"));

		case 81:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_94"));

		case 82:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_95"));

		/*case 86:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("PM_SPAWN_Y"));*/

		case 87:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF1"));

		case 103:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF1"))), fmt::arg("num", "1"));
		case 104:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF1"))), fmt::arg("num", "2"));
		case 105:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF1"))), fmt::arg("num", "3"));

		case 88:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF2"));

		case 106:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF2"))), fmt::arg("num", "1"));
		case 107:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF2"))), fmt::arg("num", "2"));
		case 108:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF2"))), fmt::arg("num", "3"));

		case 89:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF3"));

		case 109:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF3"))), fmt::arg("num", "1"));
		case 110:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF3"))), fmt::arg("num", "2"));
		case 111:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF3"))), fmt::arg("num", "3"));

		case 90:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF4"));

		case 112:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF4"))), fmt::arg("num", "1"));
		case 113:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF4"))), fmt::arg("num", "2"));
		case 114:
			return LANG_FMT("OFFICE_GARAGE", fmt::arg("office", Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_OFF4"))), fmt::arg("num", "3"));

		case 91:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH1"));

		case 92:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH2"));

		case 93:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH3"));

		case 94:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH4"));

		case 95:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH5"));

		case 96:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH6"));

		case 97:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH7"));

		case 98:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH8"));

		case 99:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH9"));

		case 100:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH10"));

		case 101:
			return std::string(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH11"))).append(" 2"); // "Plateo Bay Clubhouse", same as MP_PROP_CLUBH5

		case 102:
			return Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("MP_PROP_CLUBH12"));
		}
	}
}
