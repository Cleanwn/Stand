#include "CommandPersonalVehicle.hpp"

#include <fmt/format.h>

#include <soup/Bitset.hpp>

#include "AbstractEntity.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandLambdaActionScript.hpp"
#include "get_current_time_millis.hpp"
#include "get_vehicle_display_name.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "PersonalVehicleCustomisationIo.hpp"
#include "Script.hpp"
#include "Util.hpp"
#include "VehicleEntityCustomisationIo.hpp"

#include "CommandSavePv.hpp"
//#include "CommandPersonalVehicleInsurance.hpp"

namespace Stand
{
	[[nodiscard]] static Label getHelpText(const uint32_t i)
	{
		auto data = PersonalVehicle{ i }.getBase();
		std::string text{};
		const char* plate = data.at(GLOBAL_PVS_PLATE_TEXT).as<const char*>();
		if (*plate != 0)
		{
			text.append(LANG_GET("PLATE")).append(": ");
			text.append(plate);
		}
		auto insurance = data.at(GLOBAL_PVS_INSURANCE).get<soup::Bitset<int>>();
		if (insurance.get(GLOBAL_PVS_INSURANCE_DESTROYED))
		{
			if (!text.empty())
			{
				text.push_back('\n');
			}
			text.append(LANG_GET("DSTRYD"));
			if (!insurance.get(GLOBAL_PVS_INSURANCE_INSURED))
			{
				text.append(" & ").append(LANG_GET("UINSRD"));
			}
		}
		else if (!insurance.get(GLOBAL_PVS_INSURANCE_INSURED))
		{
			if (!text.empty())
			{
				text.push_back('\n');
			}
			text.append(LANG_GET("UINSRD"));
		}
		return LIT(std::move(text));
	}

	CommandPersonalVehicle::CommandPersonalVehicle(CommandList* parent, const uint32_t i, const Hash model)
		: CommandWithEntityPreview(parent, LIT(get_vehicle_display_name_no_manufacturer(model)), CMDNAMES("pv"), getHelpText(i), CMDFLAGS_LIST | CMDFLAG_TEMPORARY | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_SKIP), i(i)
	{
		this->command_names.at(0).append(utf8ToCmdName(fmt::to_string(i)));

		this->createChild<CommandLambdaAction>(LOC("REQNCALL"), getSuffixedCommandNames(CMDNAME("call")), NOLABEL, [this](Click& click)
		{
			click.ensureYieldableScriptThread([this]
			{
				requestAndAwait(true).call();
			});
		}, CMDFLAG_TEMPORARY);
		this->createChild<CommandLambdaAction>(LOC("REQNDRV"), getSuffixedCommandNames(CMDNAME("drive")), NOLABEL, [this](Click& click)
		{
			click.ensureYieldableScriptThread([this]
			{
				PED::SET_PED_INTO_VEHICLE(g_player_ped, requestAndAwait(), -1);
			});
		}, CMDFLAG_TEMPORARY);
		this->createChild<CommandLambdaAction>(LOC("REQ"), getSuffixedCommandNames(CMDNAME("request")), NOLABEL, [this](Click& click)
		{
			click.ensureYieldableScriptThread([this]
			{
				request();
			});
		}, CMDFLAG_TEMPORARY);
		/*this->createChild<CommandLambdaAction>(LOC("PVRPLC"), {}, LOC("PVRPLC_H"), [this](Click& click)
		{
			click.ensureScriptThread([this, click]() mutable
			{
				auto veh = Util::getCurrentlyDrivingVehicle(click);
				if (veh.isValid())
				{
					VehicleEntityCustomisationIo ent_cust{ veh };
					PersonalVehicleCustomisationIo pv_cust{ this->i };
					ent_cust.copyTo(pv_cust);
				}
			});
		}, CMDFLAG_TEMPORARY);*/
		this->createChild<CommandLambdaActionScript>(LOC("SPWN"), getSuffixedCommandNames(CMDNAME("spawn")), NOLABEL, [this](const Click& _click)
		{
			PersonalVehicleCustomisationIo veh_cio{ this->i };
			Click click(_click);
			veh_cio.spawnClone(click);
		}, CMDFLAG_TEMPORARY);
		this->createChild<CommandSavePv>();
		/*this->createChild<CommandLambdaAction>(LOC("INSURCLM"), getSuffixedCommandNames(CMDNAME("claim")), NOLABEL, [this](Click& click)
		{
			click.ensureYieldableScriptThread([this]
			{
				PersonalVehicle{ this->i }.setInsuranceFlag(GLOBAL_PVS_INSURANCE_DESTROYED, false);
				PersonalVehicle::saveStats();
				updateHelpText();
			});
		}, CMDFLAG_TEMPORARY);
		this->createChild<CommandPersonalVehicleInsurance>();*/
		if (model != ATSTRINGHASH("avenger")
			&& model != ATSTRINGHASH("trailerlarge") // MOC
			&& model != ATSTRINGHASH("terbyte")
			&& model != ATSTRINGHASH("thruster")
			&& model != ATSTRINGHASH("rcbandito")
			&& model != ATSTRINGHASH("minitank")
			)
		{
			this->createChild<CommandLambdaActionScript>(LOC("DEL"), {}, NOLABEL, [this](const Click& click)
			{
				ScriptGlobal(GLOBAL_PVS).at(this->i, GLOBAL_PVS_ELMSIZE).at(GLOBAL_PVS_MODEL).set<Hash>(ATSTRINGHASH("ruffian"));
				ScriptGlobal(GLOBAL_PVS).at(this->i, GLOBAL_PVS_ELMSIZE).at(GLOBAL_PVS_INSURANCE).as<soup::Bitset<int>*>()->set(GLOBAL_PVS_INSURANCE_INSURED, false);
				requestAndAwait().destroy();
			}, CMDFLAG_TEMPORARY);
		}
	}

	void CommandPersonalVehicle::updateHelpText()
	{
		setHelpText(getHelpText(i));
	}

	Hash CommandPersonalVehicle::getModelHash()
	{
		return getModelHashConst();
	}

	Hash CommandPersonalVehicle::getModelHashConst() const
	{
		return ScriptGlobal(GLOBAL_PVS).at(i, GLOBAL_PVS_ELMSIZE).at(GLOBAL_PVS_MODEL).get<Hash>();
	}

	AbstractEntity CommandPersonalVehicle::createPreviewEntity()
	{
		PersonalVehicleCustomisationIo cust_io{ i };
		return cust_io.createEntityNow(false);
	}

	void CommandPersonalVehicle::request(bool reuse_if_out)
	{
		auto pv = AbstractEntity::getPersonalVehicle();
		if (pv.isValid())
		{
			if (reuse_if_out && ScriptGlobal(GLOBAL_MOST_RECENT_PV_INDEX).get<int>() == i)
			{
				return;
			}
			pv.makePassengersLeave();
		}
		if (getModelHashConst() == ATSTRINGHASH("oppressor2"))
		{
			ScriptGlobal(GLOBAL_MKII_COOLDOWN).set(0);
		}
		if (ScriptGlobal(GLOBAL_PVS).at(i, GLOBAL_PVS_ELMSIZE).at(GLOBAL_PVS_INSURANCE).as<soup::Bitset<int>*>()->get(GLOBAL_PVS_INSURANCE_DESTROYED))
		{
			ScriptGlobal(GLOBAL_PVS).at(i, GLOBAL_PVS_ELMSIZE).at(GLOBAL_PVS_INSURANCE).as<soup::Bitset<int>*>()->set(GLOBAL_PVS_INSURANCE_DESTROYED, false);
			updateHelpText();
		}

		ScriptGlobal(MPGlobalsAmbience_iPVSlotOfficePA).set(i);
		ScriptGlobal(MPGlobalsAmbience_bRequestedByOfficePA).set(TRUE);
		Script::current()->yield(700);
		if (!AbstractEntity::getPersonalVehicle().isValid())
		{
			ScriptGlobal(MPGlobalsAmbience_iVDPersonalVehicleSlot).set(i);
			ScriptGlobal(MPGlobalsAmbience_bLaunchVehicleDropPersonal).set(TRUE);
		}
	}

	AbstractEntity CommandPersonalVehicle::requestAndAwait(bool reuse_if_out)
	{
		request(reuse_if_out);
		time_t deadline = get_current_time_millis() + 6000;
		AbstractEntity pv;
		do 
		{
			Script::current()->yield();
			pv = AbstractEntity::getPersonalVehicle();
		} while (!pv.isValid() && get_current_time_millis() < deadline);
		return pv;
	}
}
