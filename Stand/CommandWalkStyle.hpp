#pragma once

#include "CommandListSelect.hpp"

#include "natives.hpp"

namespace Stand
{
	struct WalkStyle
	{
		const char* clipset;
		Label menu_name;
		std::vector<CommandName> command_names;
	};

	inline WalkStyle walk_styles[] = {
		{"move_f@arrogant@a", LOC("WLKSTL_50"), CMDNAMES("arrogant")},
		{"move_m@bag", LOC("WLKSTL_14"), CMDNAMES("bag")}, // Bag
		{"MOVE_M@BAIL_BOND_NOT_TAZERED", LOC("WLKSTL_15"), CMDNAMES("bailbold")}, // Bail Bond
		{"MOVE_M@BAIL_BOND_TAZERED", LOC("WLKSTL_16"), CMDNAMES("tazered")}, // Bail Bond (Tazered)
		{"move_m@brave", LOC("WLKSTL_17"), CMDNAMES("brave")}, // Brave
		{"move_ped_bucket", LOC("WLKSTL_33"), CMDNAMES("bucket")}, // Bucket
		{"ANIM_GROUP_MOVE_BALLISTIC", LOC("WLKSTL_1"), CMDNAMES("ballistic")}, // Ballistic
		{"move_m@buzzed", LOC("WLKSTL_44"), CMDNAMES("buzzed")}, // Buzzed
		{"move_m@casual@d", LOC("WLKSTL_18"), CMDNAMES("casusal")}, // Casual
		{"move_m@confident", LOC("WLKSTL_51"), CMDNAMES("confident")}, // Confident
		{"move_ped_crouched", LOC("WLKSTL_34"), CMDNAMES("crouched")}, // Crouched
		{"move_f@flee@a", LOC("WLKSTL_8"), CMDNAMES("fleeing")}, // Fleeing
		{"move_m@gangster@var_i", LOC("WLKSTL_26"), CMDNAMES("gangster")}, // Gangster
		{"move_m@gangster@var_f", LOC("WLKSTL_25"), CMDNAMES("shygangster")}, // Gangster (Shy)
		{"move_m@gangster@var_e", LOC("WLKSTL_24"), CMDNAMES("slowgangster")}, // Gangster (Slow)
		{"move_m@prisoner_cuffed", LOC("WLKSTL_46"), CMDNAMES("handcuffed")}, // Handcuffed
		{"move_f@heels@c", LOC("WLKSTL_52"), CMDNAMES("heels")}, // Heels
		{"move_m@hiking", LOC("WLKSTL_53"), CMDNAMES("hiking")}, // Hiking
		{"move_m@hipster@a", LOC("WLKSTL_56"), CMDNAMES("hipster")}, // Hipster
		{"clipset@move@trash_fast_turn", LOC("WLKSTL_3"), CMDNAMES("trash")}, // Holding Trash
		{"move_injured_generic", LOC("WLKSTL_12"), CMDNAMES("injured")}, // Injured
		{"move_m@JOG@", LOC("WLKSTL_27"), CMDNAMES("jogging")}, // Jogging
		{"move_ped_mop", LOC("WLKSTL_35"), CMDNAMES("mop")}, // Mop
		{"MOVE_M@PRISON_GAURD", LOC("WLKSTL_28"), CMDNAMES("prisonguard")}, // Prison Guard
		{"move_m@business@a", LOC("WLKSTL_45"), CMDNAMES("relaxed")}, // Relaxed
		{"FEMALE_FAST_RUNNER", LOC("WLKSTL_4"), CMDNAMES("running")}, // Running
		{"move_m@sad@a", LOC("WLKSTL_47"), CMDNAMES("sad")}, // Sad
		{"move_m@sassy", LOC("WLKSTL_48"), CMDNAMES("sassymale")}, // Sassy 1
		{"move_f@sassy", LOC("WLKSTL_49"), CMDNAMES("sassyfemale")}, // Sassy 2
		{"move_f@scared", LOC("WLKSTL_9"), CMDNAMES("scared")}, // Scared
		{"move_f@sexy@a", LOC("WLKSTL_10"), CMDNAMES("sexy")}, // Sexy
		{"move_m@shadyped@a", LOC("WLKSTL_55"), CMDNAMES("shady")}, // Shady
		{"move_m@fire", LOC("WLKSTL_23"), CMDNAMES("tough")}, // Tough

		// Drunk
		{"MOVE_M@DRUNK@SLIGHTLYDRUNK", LOC("WLKSTL_21"), CMDNAMES("slightlydrunk")}, // Slightly Drunk
		{"MOVE_M@DRUNK@MODERATEDRUNK", LOC("WLKSTL_19"), CMDNAMES("moderatelydrunk")}, // Moderately Drunk
		{"MOVE_M@DRUNK@MODERATEDRUNK_HEAD_UP", LOC("WLKSTL_20"), CMDNAMES("moderatelydrunkheadup")}, // Moderately Drunk (Head Up)
		{"MOVE_M@DRUNK@VERYDRUNK", LOC("WLKSTL_22"), CMDNAMES("verydrunk", "drunk")}, // Very Drunk
		// Characters
		{"MOVE_P_M_ONE", LIT("Franklin"), CMDNAMES("franklin")}, // Franklin
		{"MOVE_P_M_ONE_BRIEFCASE", LOC("WLKSTL_30"), CMDNAMES("briefcase")}, // Franklin (Briefcase)
		{"move_characters@franklin@fire", LOC("WLKSTL_5"), CMDNAMES("franklintough")}, // Franklin (Tough)
		{"move_characters@Jimmy@slow@", LIT("Jimmy"), CMDNAMES("jimmy")}, // Jimmy
		{"ANIM_GROUP_MOVE_LEMAR_ALLEY", LIT("Lamar"), CMDNAMES("lamar")}, // Lamar
		{"move_heist_lester", LIT("Lester"), CMDNAMES("lester")}, // Lester
		{"move_lester_CaneUp", LOC("WLKSTL_13"), CMDNAMES("cane")}, // Lester (Cane)
		{"move_characters@michael@fire", LIT("Michael"), CMDNAMES("micheal", "michael")}, // Michael
		{"move_p_m_zero_janitor", LOC("WLKSTL_31"), CMDNAMES("janitor")}, // Michael (Janitor)
		{"move_p_m_zero_slow", LOC("WLKSTL_32"), CMDNAMES("slowmicheal", "slowmichael")}, // Michael (Slow)
		// Legit Shit
		{"MOVE_M@FEMME@", LOC("WLKSTL_36"), CMDNAMES("femmemale")}, // Femme (Male)
		{"MOVE_F@FEMME@", LOC("WLKSTL_37"), CMDNAMES("femmefemale")}, // Femme (Female)
		{"MOVE_M@GANGSTER@NG", LOC("WLKSTL_38"), CMDNAMES("malegangster")}, // Gangster (Male)
		{"MOVE_F@GANGSTER@NG", LOC("WLKSTL_39"), CMDNAMES("femalegangster")}, // Gangster (Female)
		{"MOVE_M@POSH@", LOC("WLKSTL_40"), CMDNAMES("poshmale")}, // Posh (Male)
		{"MOVE_F@POSH@", LOC("WLKSTL_41"), CMDNAMES("poshfemale")}, // Posh (Female)
		{"MOVE_M@TOUGH_GUY@", LOC("WLKSTL_42"), CMDNAMES("maletoughguy")}, // Tough Guy (Male)
		{"MOVE_F@TOUGH_GUY@", LOC("WLKSTL_43"), CMDNAMES("femaletoughguy")}, // Tough Guy (Female)
	};

	class CommandWalkStyle : public CommandListSelect
	{
	public:
		static inline bool walk_style_overridden = false;

		[[nodiscard]] static std::vector<CommandListActionItemData> getOptions()
		{
			std::vector<CommandListActionItemData> options = {
				{0, LOC("NRML"), CMDNAMES("normal")}
			};
			long long i = 0;
			for (const auto& ws : walk_styles)
			{
				options.emplace_back(CommandListActionItemData{ ++i, ws.menu_name, ws.command_names });
			}
			return options;
		}

		explicit CommandWalkStyle(CommandList* const parent)
			: CommandListSelect(parent, LOC("WLKSTL"), CMDNAMES("walkstyle"), NOLABEL, getOptions(), 0, CMDFLAGS_LIST_SELECT, CMDFLAG_FEATURELIST_FINISHLIST)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			const auto value = this->value;
			if (value == 0)
			{
				ensureScriptThread(click, []
				{
					if (!walk_style_overridden)
					{
						PED::RESET_PED_MOVEMENT_CLIPSET(g_player_ped, 0.0f);
					}
				});
			}
			else
			{
				const char* const walk_style = walk_styles[value - 1].clipset;
				registerScriptTickEventHandler(click, [=]()
				{
					if (value == this->value)
					{
						if (!walk_style_overridden)
						{
							if (STREAMING::HAS_CLIP_SET_LOADED(walk_style))
							{
								PED::SET_PED_MOVEMENT_CLIPSET(g_player_ped, walk_style, 0.0f);
								if (value == 34)
								{
									PED::SET_PED_STRAFE_CLIPSET(g_player_ped, "move_ped_crouched_strafing");
									if (g_player_ped.isAiming())
									{
										PED::SET_MOVEMENT_MODE_OVERRIDE(g_player_ped, "-");
									}
									else
									{
										//PED::SET_MOVEMENT_MODE_OVERRIDE(g_player_ped, "DEFAULT_ACTION");
									}
								}
								else
								{
									PED::RESET_PED_STRAFE_CLIPSET(g_player_ped);
								}
							}
							else
							{
								STREAMING::REQUEST_CLIP_SET(walk_style);
							}
						}
						return true;
					}
					return false;
				});
			}
		}
	};
}
