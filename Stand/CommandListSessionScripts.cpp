#include "CommandListSessionScripts.hpp"

#include "CommandDivider.hpp"
#include "CommandSessionScript.hpp"
#include "eMpMission.hpp"

#include "CommandStartSessionScript.hpp"

#include "CommandSessionScriptBb.hpp"
#include "CommandSessionScriptBb2.hpp"
#include "CommandSessionScriptBeast.hpp"
#include "CommandSessionScriptCpCollection.hpp"
#include "CommandSessionScriptChallenges.hpp"
#include "CommandSessionScriptCriminalDamage.hpp"
#include "CommandSessionScriptFmintro.hpp"
#include "CommandSessionScriptPennedin.hpp"

namespace Stand
{
	CommandListSessionScripts::CommandListSessionScripts(CommandList* const parent)
		: CommandListWithOnTickAsActiveList(parent, LOC("SESSSCRS"))
	{
		// Run Script
		{
			auto launcher = this->createChild<CommandList>(LOC("RUNSCR"), { CMDNAME("runscript"), CMDNAME("runsessionscript") });

			{
				auto events = launcher->createChild<CommandList>(LOC("SCRCAT_FA_P"));

				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("BUSINESS_BATTLES"), CMDFLAG_FEATURELIST_FINISHLIST);
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_CHALLENGES"));
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_CP_COLLECTION"));
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_CRIMINAL_DAMAGE"));
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_PASS_THE_PARCEL"));
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_HOT_PROPERTY"));
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_HUNT_THE_BEAST"));
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_KILL_LIST"));
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_KING_OF_THE_CASTLE"));
				events->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_PENNED_IN"));

			}

			{
				auto arcade_games = launcher->createChild<CommandList>(LOC("SCRCAT_AG_P"));

				arcade_games->createChild<CommandStartSessionScript>(ATSTRINGHASH("gunslinger_arcade"), CMDFLAG_FEATURELIST_FINISHLIST);
				arcade_games->createChild<CommandStartSessionScript>(ATSTRINGHASH("camhedz_arcade"));
				arcade_games->createChild<CommandStartSessionScript>(ATSTRINGHASH("puzzle"));
				arcade_games->createChild<CommandStartSessionScript>(ATSTRINGHASH("road_arcade"));
				arcade_games->createChild<CommandStartSessionScript>(ATSTRINGHASH("ggsm_arcade"));
				arcade_games->createChild<CommandStartSessionScript>(ATSTRINGHASH("wizard_arcade"));
			}

			{
				auto missions = launcher->createChild<CommandList>(LOC("SCRCAT_RFA_P"));

				missions->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_DEAD_DROP"), CMDFLAG_FEATURELIST_FINISHLIST);
				missions->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_HOT_TARGET"));
				missions->createChild<CommandDivider>(LOC("SCRCAT_MISS_P"));
				missions->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_PLANE_TAKEDOWN"));
				missions->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_DISTRACT_COPS"));
				missions->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_DESTROY_VEH"));
			}

			{
				auto breaking = launcher->createChild<CommandList>(LOC("SCRCAT_GRIEF"));

				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("FM_INTRO"), CMDFLAG_FEATURELIST_FINISHLIST);
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("fm_mission_controller"));
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("golf_mp"));
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("FM_Survival_Controller"));
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("fm_deathmatch_controler"));
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("tennis_network_mp"));
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("Range_Modern_MP"));
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("FM_Race_Controler"));
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("fm_Bj_race_controler"));
				breaking->createChild<CommandStartSessionScript>(ATSTRINGHASH("fm_content_ufo_abduction"));
			}

			{
				auto services = launcher->createChild<CommandList>(LOC("SCRCAT_SERV_P"));

				services->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_airstrike"));
				services->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_AMMO_DROP"));
				services->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_backup_heli"));
				services->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_ISLAND_BACKUP_HELI"));
				services->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_BOAT_TAXI"));
				services->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_HELI_TAXI"));
				services->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_BRU_BOX"));
			}

			launcher->createChild<CommandStartSessionScript>(ATSTRINGHASH("MG_RACE_TO_POINT"));
			launcher->createChild<CommandStartSessionScript>(ATSTRINGHASH("Pilot_School_MP"));
			launcher->createChild<CommandStartSessionScript>(ATSTRINGHASH("fm_content_sightseeing"));
			launcher->createChild<CommandStartSessionScript>(ATSTRINGHASH("fm_hideout_controler"));
			launcher->createChild<CommandStartSessionScript>(ATSTRINGHASH("AM_Darts"));
			launcher->createChild<CommandStartSessionScript>(ATSTRINGHASH("FM_Impromptu_DM_Controler"));
			launcher->createChild<CommandStartSessionScript>(ATSTRINGHASH("fm_content_slasher"));

			{
				auto cutscenes = launcher->createChild<CommandList>(LOC("SCRCAT_CUT_P"));

				cutscenes->createChild<CommandStartSessionScript>(ATSTRINGHASH("am_heist_int"), CMDFLAG_FEATURELIST_FINISHLIST);
				cutscenes->createChild<CommandStartSessionScript>(ATSTRINGHASH("am_lester_cut"));
				cutscenes->createChild<CommandStartSessionScript>(ATSTRINGHASH("am_ronTrevor_Cut"));
			}

			{
				auto uncategorised = launcher->createChild<CommandList>(LOC("NOCAT"));

				uncategorised->createChild<CommandStartSessionScript>(ATSTRINGHASH("fm_content_business_battles"), CMDFLAG_FEATURELIST_FINISHLIST);

				// VIP Work
				uncategorised->createChild<CommandStartSessionScript>(ATSTRINGHASH("GB_HEADHUNTER"));
				uncategorised->createChild<CommandStartSessionScript>(ATSTRINGHASH("GB_DEATHMATCH"));
				uncategorised->createChild<CommandStartSessionScript>(ATSTRINGHASH("GB_HUNT_THE_BOSS"));
				uncategorised->createChild<CommandStartSessionScript>(ATSTRINGHASH("GB_SIGHTSEER"));
				uncategorised->createChild<CommandStartSessionScript>(ATSTRINGHASH("GB_YACHT_ROB"));

				// VIP Challenges
				uncategorised->createChild<CommandStartSessionScript>(ATSTRINGHASH("GB_POINT_TO_POINT"));
				uncategorised->createChild<CommandStartSessionScript>(ATSTRINGHASH("GB_CASHING_OUT"));

				for (int i = 1; i < NUM_SCRIPT_IDS; ++i)
				{
					const auto hash = SessionScriptMgr::id_to_hash(i);
					if (hash != 0
						&& !CommandStartSessionScript::categorised.contains(hash)
						)
					{
						uncategorised->createChild<CommandStartSessionScript>(hash);
					}
				}
				CommandStartSessionScript::categorised.clear();
			}
		}

		running_scripts_divider = this->createChild<CommandDivider>(LOC("RUNSCRS"));
		running_scripts_offset = this->children.size();

		this->createChild<CommandSessionScriptBb>();
		this->createChild<CommandSessionScriptBb2>();
		this->createChild<CommandSessionScriptBeast>();
		this->createChild<CommandSessionScriptCpCollection>();
		this->createChild<CommandSessionScriptChallenges>();
		this->createChild<CommandSessionScriptCriminalDamage>();
		this->createChild<CommandSessionScriptFmintro>();
		this->createChild<CommandSessionScriptPennedin>();
	}

	void CommandListSessionScripts::onTickAsActiveList()
	{
		bool changed_visibility = false;
		bool have_any_running_script = false;
		for (auto i = this->children.cbegin() + running_scripts_offset; i != this->children.cend(); ++i)
		{
			auto cmd = (*i)->as<CommandSessionScript>();
			if (cmd->getThread() == nullptr)
			{
				changed_visibility |= setVisibleBulk(cmd, false);
				continue;
			}
			have_any_running_script = true;
			changed_visibility |= setVisibleBulk(cmd, true);
		}
		changed_visibility |= setVisibleBulk(running_scripts_divider, have_any_running_script);
		if (changed_visibility)
		{
			processChildrenVisibilityUpdate();
		}
	}
}
