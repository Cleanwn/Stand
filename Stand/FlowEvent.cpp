#include "FlowEvent.hpp"
#include "FlowEvent.hpp"

#include "Label.hpp"

namespace Stand
{
	Label FlowEvent::getName(flowevent_t id) noexcept
	{
		switch (id)
		{
		case NE_INFRING: return LOC("INFRING");
		case NE_CAMSHAKE: return LOC("CAMSHAKE");
		case NE_RAGDOLL: return LOC("RAGDOLLEVT");
		case NE_SUDDENDEATH: return LOC("MODDMGEVT");
		case NE_DISARM: return LOC("REMOVE_WEAPON_EVENT");
		case SE_CRASH: return LOC("PTX_CRSHE");
		case SE_KICK: return LOC("PTX_KICK");
		case SE_INVALID: return LOC("PTX_INV");
		case SE_UNUSUAL: return LOC("PTX_WEIRD");
		case SE_STARTFMMISSION: return LOC("MISSIONEVT");
		case SE_STARTFMMISSION_NCEO: return LOC("MISSIONEVT_NCEO");
		case SE_INTERIORWARP: return LOC("INTTPEVT");
		case SE_INTERIORWARP_NCEO: return LOC("INTTPEVT_NCEO");
		case SE_GIVECOLLE: return LOC("GIVECOLLE");
		case SE_GIVECOLLE_NCEO: return LOC("GIVECOLLE_NCEO");
		case SE_INFLOAD: return LOC("INFLOAD");
		case SE_CAYO_INV: return LOC("PTX_CAYO");
		case SE_APTINV: return LOC("PTX_APT");
		case SE_SENDTOJOB: return LOC("SND2JB");
		case SE_CEOKICK: return LOC("CEOKICK");
		case NE_VEHTAKEOVER: return LOC("PTX_DRVCTRL");
		case SE_DISABLEDRIVINGVEHICLES: return LOC("PLYDDVEH");
		case SE_PVKICK: return LOC("VEHKICK");
		case SE_FORCECAMFRONT: return LOC("PLYFRCECAMFRNT");
		case SE_FREEZE: return LOC("FRZ");
		case SE_NOTIFY_EMPTY: return LOC("EMTYNTFY");
		case SE_INTERIORKICK: return LOC("INTKICK");
		case SE_TRIGGERDEFEND: return LOC("TRIGDEFD");
		case CLSFN_MOD: return LOC("FLAG_MOD");
		case CLSFN_AMN: return LOC("FLAG_AMN");
#if HAS_MODAMN
		case CLSFN_MODAMN: return LOC("FLAG_MODAMN");
#endif
		case CLSFN_LM: return LOC("CLSFN_LM");
		case CLSFN_PM: return LOC("CLSFN_PM");
		case MOD_ID_STAND: return LOC("DT_SU");
		case MOD_ID_STAND_COLOADING: return LOC("DT_SU_C");
		case MOD_ID_OTHER: return LOC("MDDRDT_SID");
		case MOD_RAC: return LOC("MDDRDT_RAC");
		case MOD_HEALTH: return LOC("MDDRDT_HLTH");
		case MOD_LONG_DEATH: return LOC("MDDRDT_LNGDED");
		case MOD_LONG_OTR: return LOC("MDDRDT_LNGOTR");
		case MOD_ARMOUR: return LOC("MDDRDT_ARMR");
		case MOD_PRESENCESPOOFING: return LOC("MDDRDT_PRES");
		case MOD_ADMINRID: return LOC("MDDRDT_RID3");
		case MOD_T2IP: return LOC("MDDRDT_T2IP");
		case MOD_HOSTKN_AGGRESSIVE: return LOC("DT_HT_2");
		case MOD_HOSTKN_SWEET: return LOC("DT_HT_1");
		case MOD_HOSTKN_HANDICAP: return LOC("DT_HT_0");
		case MOD_SPFDHOSTKN: return LOC("DT_HT_3");
		case MOD_MODEL: return LOC("MDDRDT_TRFRM");
		case MOD_GODATK: return LOC("MDDRDT_GODATK");
		case MOD_DMGMUL: return LOC("DMG");
		case MOD_FROG: return LOC("FROG");
		case MOD_RSDEV: return LOC("MDDRDT_RSDEV");
		case MOD_RSQA: return LOC("MDDRDT_RSQA");
		case MOD_CHEAT: return LOC("MDDRDT_CHEAT");
		case MOD_RP2: return LOC("MDDRDT_RP2");
		case MOD_RP3: return LOC("MDDRDT_RP3");
		case MOD_RP4: return LOC("MDDRDT_RP4");
		case MOD_NONET: return LOC("MDDRT_NONET");
		case MOD_DRIVEB: return LOC("MDDRT_DRIVEB");
		case MOD_SILENTCHAT: return LOC("MDDRT_SILENTCHAT");
		case MOD_BOUNTYSPAM: return LOC("MDDRT_BOUNTYSPAM");
		case MOD_SUSSYBOUNTY: return LOC("MDDRT_SUSSYBOUNTY");
		case MOD_EXPBLAME: return LOC("MDDRT_EXPBLAME");
		case MOD_BADEXPLOSION: return LOC("MDDRT_EXPL");
		case MOD_ORGNAME: return LOC("MOD_ORGNAME");
		case MOD_CAYO: return LOC("MDDRT_CAYO");
		case MOD_LLLUBE: return LOC("LLLUBE");
		case MOD_MISC: return LOC("MDDRT_MISC");
#ifdef STAND_DEBUG
		case MOD_INCONGRUENCE_1: return LIT("Incongruence 1");
		case MOD_INCONGRUENCE_2: return LIT("Incongruence 2");
#endif

		case SMS_ANY: return LOC("SMS2");

		case CHAT_AD:
		case SMS_AD:
			return LOC("CHT_AD");

		case CHAT_PROFANITYBYPASS:
		case SMS_PROFANITYBYPASS:
			return LOC("MSGFILTBP");

		case CHAT_SPAM: return LOC("CHTSPM");
		case SYNCIN_IMS: return LOC("PTX_IMS");
		case SYNCIN_WOS: return LOC("PTX_WOS");
		case SYNCIN_CAGE: return LOC("CGE");
		case SYNCIN_ATTACH: return LOC("PTX_ATTACH");
		case SYNCIN_CLONE_CREATE: return LOC("PTX_S_CC");
		case SYNCIN_CLONE_UPDATE: return LOC("PTX_S_CU");
		case SYNCIN_CLONE_DELETE: return LOC("PTX_S_CD");
		case SYNCIN_ACK_CLONE_CREATE: return LOC("PTX_S_ACC");
		case SYNCIN_ACK_CLONE_UPDATE: return LOC("PTX_S_ACU");
		case SYNCIN_ACK_CLONE_DELETE: return LOC("PTX_S_ACD");
		case SYNCIN_CONTROL: return getName(GIVE_CONTROL_EVENT);
		case SYNCOUT_CLONE_CREATE: return LOC("PTX_S_CC");
		case SYNCOUT_CLONE_UPDATE: return LOC("PTX_S_CU");
		case SYNCOUT_CLONE_DELETE: return LOC("PTX_S_CD");
		case SCRLAUNCH_NOCAT: return LOC("NOCAT");
		case SCRLAUNCH_FE: return LOC("SCRCAT_FA");
		case SCRLAUNCH_AG: return LOC("SCRCAT_AG");
		case SCRLAUNCH_OFM: return LOC("SCRCAT_RFA");
		case SCRLAUNCH_GRIEF: return LOC("SCRCAT_GRIEF");
		case SCRLAUNCH_SERV: return LOC("SCRCAT_SERV");
		case SCRLAUNCH_FOIM: return LOC("SCR_FRCEPI");
		case SCRLAUNCH_PS: return LOC("SCR_PS");
		case SCRLAUNCH_STRIKE: return LOC("SCR_STRIKE");
		case SCRLAUNCH_DPSSV: return LOC("SCR_DPSSV");
		case SCRLAUNCH_DARTS: return LOC("SCR_DARTS");
		case SCRLAUNCH_IMPDM: return LOC("SCR_IMPDM");
		case SCRLAUNCH_SLASHER: return LOC("SCR_SLASHER");
		case SCRLAUNCH_CUT: return LOC("SCRCAT_CUT");
		case PUP_CASH: return LOC("PTX_CASHPUP");
		case PUP_RP: return LOC("PTX_RPPUP");
		case PUP_INVALID: return LOC("PTX_IPUP");
		case MISC_PTFX: return LOC("PLRPTFXSPM");

		case CUSTOM_1:
		case CUSTOM_2:
		case CUSTOM_3:
		case CUSTOM_4:
		case CUSTOM_5:
		case CUSTOM_6:
		case CUSTOM_7:
		case CUSTOM_8:
		case CUSTOM_9:
		case CUSTOM_10:
		case CUSTOM_11:
		case CUSTOM_12:
		case CUSTOM_13:
		case CUSTOM_14:
		case CUSTOM_15:
		case CUSTOM_16:
		case CUSTOM_17:
		case CUSTOM_18:
		case CUSTOM_19:
		case CUSTOM_20:
			return custom_event_names[id - _CUSTOM_BEGIN];
		}
		return get_net_event_name(id);
	}

	Label FlowEvent::getName(flowevent_t id, const std::string& extra_data) noexcept
	{
		return getName(getName(id), extra_data);
	}

	Label FlowEvent::getName(Label name, const std::string& extra_data) noexcept
	{
		if (!extra_data.empty())
		{
			name.makeLiteralLocalised();
			name.literal_str.append(" (");
			name.literal_str.append(extra_data);
			name.literal_str.push_back(')');
		}
		return name;
	}
}
