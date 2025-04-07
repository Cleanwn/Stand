#include "CommandHudColourEditor.hpp"

#include "CommandColour.hpp"
#include "CommandColourSlider.hpp"
#include "HudColour.hpp"
#include "natives.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	struct CommandHudColourEditorColour : public CommandColour
	{
	private:
		const int id;

	public:
		explicit CommandHudColourEditorColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, int id, int default_r, int default_g, int default_b, int default_a)
			: CommandColour(parent, std::move(menu_name), std::move(command_names), NOLABEL, default_r, default_g, default_b, default_a, CMDFLAGS_COLOUR | CMDFLAG_FEATURELIST_FINISHLIST), id(id)
		{
		}

		void onChange(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				HUD::REPLACE_HUD_COLOUR_WITH_RGBA(id, r->value, g->value, b->value, a->value);
			});
		}
	};

	CommandHudColourEditor::CommandHudColourEditor(CommandList* const parent)
		: CommandList(parent, LOC("EDTHCLR"))
	{
		this->createColourChild("FREEMODE", HUD_COLOUR_FREEMODE);
		this->createColourChild("MICHAEL", HUD_COLOUR_MICHAEL);
		this->createColourChild("MICHAEL_DARK", HUD_COLOUR_MICHAEL_DARK);
		this->createColourChild("FRANKLIN", HUD_COLOUR_FRANKLIN);
		this->createColourChild("FRANKLIN_DARK", HUD_COLOUR_FRANKLIN_DARK);
		this->createColourChild("TREVOR", HUD_COLOUR_TREVOR);
		this->createColourChild("TREVOR_DARK", HUD_COLOUR_TREVOR_DARK);

		this->createColourChild("RADAR_HEALTH", HUD_COLOUR_RADAR_HEALTH);
		this->createColourChild("RADAR_ARMOUR", HUD_COLOUR_RADAR_ARMOUR);
		this->createColourChild("RADAR_DAMAGE", HUD_COLOUR_RADAR_DAMAGE);
		this->createColourChild("WAYPOINT", HUD_COLOUR_WAYPOINT);

		this->createColourChild("SIMPLEBLIP_DEFAULT", HUD_COLOUR_SIMPLEBLIP_DEFAULT);
		this->createColourChild("MENU_BLUE", HUD_COLOUR_MENU_BLUE);
		this->createColourChild("MENU_GREY_LIGHT", HUD_COLOUR_MENU_GREY_LIGHT);
		this->createColourChild("MENU_BLUE_EXTRA_DARK", HUD_COLOUR_MENU_BLUE_EXTRA_DARK);
		this->createColourChild("MENU_YELLOW", HUD_COLOUR_MENU_YELLOW);
		this->createColourChild("MENU_YELLOW_DARK", HUD_COLOUR_MENU_YELLOW_DARK);
		this->createColourChild("MENU_GREEN", HUD_COLOUR_MENU_GREEN);
		this->createColourChild("MENU_GREY", HUD_COLOUR_MENU_GREY);
		this->createColourChild("MENU_GREY_DARK", HUD_COLOUR_MENU_GREY_DARK);
		this->createColourChild("MENU_HIGHLIGHT", HUD_COLOUR_MENU_HIGHLIGHT);
		this->createColourChild("MENU_STANDARD", HUD_COLOUR_MENU_STANDARD);
		this->createColourChild("MENU_DIMMED", HUD_COLOUR_MENU_DIMMED);
		this->createColourChild("MENU_EXTRA_DIMMED", HUD_COLOUR_MENU_EXTRA_DIMMED);
		this->createColourChild("BRIEF_TITLE", HUD_COLOUR_BRIEF_TITLE);
		this->createColourChild("MID_GREY_MP", HUD_COLOUR_MID_GREY_MP);
		this->createColourChild("BRONZE", HUD_COLOUR_BRONZE);
		this->createColourChild("SILVER", HUD_COLOUR_SILVER);
		this->createColourChild("GOLD", HUD_COLOUR_GOLD);
		this->createColourChild("PLATINUM", HUD_COLOUR_PLATINUM);
		this->createColourChild("GANG1", HUD_COLOUR_GANG1);
		this->createColourChild("GANG2", HUD_COLOUR_GANG2);
		this->createColourChild("GANG3", HUD_COLOUR_GANG3);
		this->createColourChild("GANG4", HUD_COLOUR_GANG4);
		this->createColourChild("SAME_CREW", HUD_COLOUR_SAME_CREW);
		this->createColourChild("PAUSE_BG", HUD_COLOUR_PAUSE_BG);
		this->createColourChild("FRIENDLY", HUD_COLOUR_FRIENDLY);
		this->createColourChild("ENEMY", HUD_COLOUR_ENEMY);
		this->createColourChild("LOCATION", HUD_COLOUR_LOCATION);
		this->createColourChild("PICKUP", HUD_COLOUR_PICKUP);
		this->createColourChild("PAUSE_SINGLEPLAYER", HUD_COLOUR_PAUSE_SINGLEPLAYER);
		this->createColourChild("FREEMODE_DARK", HUD_COLOUR_FREEMODE_DARK);
		this->createColourChild("INACTIVE_MISSION", HUD_COLOUR_INACTIVE_MISSION);
		this->createColourChild("DAMAGE", HUD_COLOUR_DAMAGE);
		this->createColourChild("PINKLIGHT", HUD_COLOUR_PINKLIGHT);
		this->createColourChild("PM_MITEM_HIGHLIGHT", HUD_COLOUR_PM_MITEM_HIGHLIGHT);
		this->createColourChild("SCRIPT_VARIABLE", HUD_COLOUR_SCRIPT_VARIABLE);
		this->createColourChild("YOGA", HUD_COLOUR_YOGA);
		this->createColourChild("TENNIS", HUD_COLOUR_TENNIS);
		this->createColourChild("GOLF", HUD_COLOUR_GOLF);
		this->createColourChild("SHOOTING_RANGE", HUD_COLOUR_SHOOTING_RANGE);
		this->createColourChild("FLIGHT_SCHOOL", HUD_COLOUR_FLIGHT_SCHOOL);
		this->createColourChild("NORTH_BLUE", HUD_COLOUR_NORTH_BLUE);
		this->createColourChild("SOCIAL_CLUB", HUD_COLOUR_SOCIAL_CLUB);
		this->createColourChild("PLATFORM_BLUE", HUD_COLOUR_PLATFORM_BLUE);
		this->createColourChild("PLATFORM_GREEN", HUD_COLOUR_PLATFORM_GREEN);
		this->createColourChild("PLATFORM_GREY", HUD_COLOUR_PLATFORM_GREY);
		this->createColourChild("FACEBOOK_BLUE", HUD_COLOUR_FACEBOOK_BLUE);
		this->createColourChild("INGAME_BG", HUD_COLOUR_INGAME_BG);
		this->createColourChild("DARTS", HUD_COLOUR_DARTS);
		this->createColourChild("GOLF_P1", HUD_COLOUR_GOLF_P1);
		this->createColourChild("GOLF_P2", HUD_COLOUR_GOLF_P2);
		this->createColourChild("GOLF_P3", HUD_COLOUR_GOLF_P3);
		this->createColourChild("GOLF_P4", HUD_COLOUR_GOLF_P4);
		this->createColourChild("WAYPOINTLIGHT", HUD_COLOUR_WAYPOINTLIGHT);
		this->createColourChild("WAYPOINTDARK", HUD_COLOUR_WAYPOINTDARK);
		this->createColourChild("PANEL_LIGHT", HUD_COLOUR_PANEL_LIGHT);
		this->createColourChild("OBJECTIVE_ROUTE", HUD_COLOUR_OBJECTIVE_ROUTE);
		this->createColourChild("PAUSEMAP_TINT", HUD_COLOUR_PAUSEMAP_TINT);
		this->createColourChild("PAUSE_DESELECT", HUD_COLOUR_PAUSE_DESELECT);
		this->createColourChild("PM_WEAPONS_PURCHASABLE", HUD_COLOUR_PM_WEAPONS_PURCHASABLE);
		this->createColourChild("PM_WEAPONS_LOCKED", HUD_COLOUR_PM_WEAPONS_LOCKED);
		this->createColourChild("END_SCREEN_BG", HUD_COLOUR_END_SCREEN_BG);
		this->createColourChild("CHOP", HUD_COLOUR_CHOP);
		this->createColourChild("PAUSEMAP_TINT_HALF", HUD_COLOUR_PAUSEMAP_TINT_HALF);
		this->createColourChild("NORTH_BLUE_OFFICIAL", HUD_COLOUR_NORTH_BLUE_OFFICIAL);
		this->createColourChild("SCRIPT_VARIABLE_2", HUD_COLOUR_SCRIPT_VARIABLE_2);
		this->createColourChild("H", HUD_COLOUR_H);
		this->createColourChild("HDARK", HUD_COLOUR_HDARK);
		this->createColourChild("T", HUD_COLOUR_T);
		this->createColourChild("TDARK", HUD_COLOUR_TDARK);
		this->createColourChild("HSHARD", HUD_COLOUR_HSHARD);
		this->createColourChild("CONTROLLER_MICHAEL", HUD_COLOUR_CONTROLLER_MICHAEL);
		this->createColourChild("CONTROLLER_FRANKLIN", HUD_COLOUR_CONTROLLER_FRANKLIN);
		this->createColourChild("CONTROLLER_TREVOR", HUD_COLOUR_CONTROLLER_TREVOR);
		this->createColourChild("CONTROLLER_CHOP", HUD_COLOUR_CONTROLLER_CHOP);
		this->createColourChild("VIDEO_EDITOR_VIDEO", HUD_COLOUR_VIDEO_EDITOR_VIDEO);
		this->createColourChild("VIDEO_EDITOR_AUDIO", HUD_COLOUR_VIDEO_EDITOR_AUDIO);
		this->createColourChild("VIDEO_EDITOR_TEXT", HUD_COLOUR_VIDEO_EDITOR_TEXT);
		this->createColourChild("HB_BLUE", HUD_COLOUR_HB_BLUE);
		this->createColourChild("HB_YELLOW", HUD_COLOUR_HB_YELLOW);
		this->createColourChild("VIDEO_EDITOR_SCORE", HUD_COLOUR_VIDEO_EDITOR_SCORE);
		this->createColourChild("VIDEO_EDITOR_AUDIO_FADEOUT", HUD_COLOUR_VIDEO_EDITOR_AUDIO_FADEOUT);
		this->createColourChild("VIDEO_EDITOR_TEXT_FADEOUT", HUD_COLOUR_VIDEO_EDITOR_TEXT_FADEOUT);
		this->createColourChild("VIDEO_EDITOR_SCORE_FADEOUT", HUD_COLOUR_VIDEO_EDITOR_SCORE_FADEOUT);
		this->createColourChild("HEIST_BACKGROUND", HUD_COLOUR_HEIST_BACKGROUND);
		this->createColourChild("VIDEO_EDITOR_AMBIENT", HUD_COLOUR_VIDEO_EDITOR_AMBIENT);
		this->createColourChild("VIDEO_EDITOR_AMBIENT_FADEOUT", HUD_COLOUR_VIDEO_EDITOR_AMBIENT_FADEOUT);
		this->createColourChild("VIDEO_EDITOR_AMBIENT_DARK", HUD_COLOUR_VIDEO_EDITOR_AMBIENT_DARK);
		this->createColourChild("VIDEO_EDITOR_AMBIENT_LIGHT", HUD_COLOUR_VIDEO_EDITOR_AMBIENT_LIGHT);
		this->createColourChild("VIDEO_EDITOR_AMBIENT_MID", HUD_COLOUR_VIDEO_EDITOR_AMBIENT_MID);
		this->createColourChild("LOW_FLOW", HUD_COLOUR_LOW_FLOW);
		this->createColourChild("LOW_FLOW_DARK", HUD_COLOUR_LOW_FLOW_DARK);
		this->createColourChild("G1", HUD_COLOUR_G1);
		this->createColourChild("G2", HUD_COLOUR_G2);
		this->createColourChild("G3", HUD_COLOUR_G3);
		this->createColourChild("G4", HUD_COLOUR_G4);
		this->createColourChild("G5", HUD_COLOUR_G5);
		this->createColourChild("G6", HUD_COLOUR_G6);
		this->createColourChild("G7", HUD_COLOUR_G7);
		this->createColourChild("G8", HUD_COLOUR_G8);
		this->createColourChild("G9", HUD_COLOUR_G9);
		this->createColourChild("G10", HUD_COLOUR_G10);
		this->createColourChild("G11", HUD_COLOUR_G11);
		this->createColourChild("G12", HUD_COLOUR_G12);
		this->createColourChild("G13", HUD_COLOUR_G13);
		this->createColourChild("G14", HUD_COLOUR_G14);
		this->createColourChild("G15", HUD_COLOUR_G15);
		this->createColourChild("ADVERSARY", HUD_COLOUR_ADVERSARY);
		this->createColourChild("DEGEN_RED", HUD_COLOUR_DEGEN_RED);
		this->createColourChild("DEGEN_YELLOW", HUD_COLOUR_DEGEN_YELLOW);
		this->createColourChild("DEGEN_GREEN", HUD_COLOUR_DEGEN_GREEN);
		this->createColourChild("DEGEN_CYAN", HUD_COLOUR_DEGEN_CYAN);
		this->createColourChild("DEGEN_BLUE", HUD_COLOUR_DEGEN_BLUE);
		this->createColourChild("DEGEN_MAGENTA", HUD_COLOUR_DEGEN_MAGENTA);
		this->createColourChild("STUNT_1", HUD_COLOUR_STUNT_1);
		this->createColourChild("STUNT_2", HUD_COLOUR_STUNT_2);
		this->createColourChild("SPECIAL_RACE_SERIES", HUD_COLOUR_SPECIAL_RACE_SERIES);
		this->createColourChild("SPECIAL_RACE_SERIES_DARK", HUD_COLOUR_SPECIAL_RACE_SERIES_DARK);
		this->createColourChild("CS", HUD_COLOUR_CS);
		this->createColourChild("CS_DARK", HUD_COLOUR_CS_DARK);
		this->createColourChild("TECH_GREEN", HUD_COLOUR_TECH_GREEN);
		this->createColourChild("TECH_GREEN_DARK", HUD_COLOUR_TECH_GREEN_DARK);
		this->createColourChild("TECH_RED", HUD_COLOUR_TECH_RED);
		this->createColourChild("TECH_GREEN_VERY_DARK", HUD_COLOUR_TECH_GREEN_VERY_DARK);
		this->createColourChild("JUNK_ENERGY", HUD_COLOUR_JUNK_ENERGY);

		this->createColourChild("NET_PLAYER1", HUD_COLOUR_NET_PLAYER1);
		this->createColourChild("NET_PLAYER1_DARK", HUD_COLOUR_NET_PLAYER1_DARK);
		this->createColourChild("NET_PLAYER2", HUD_COLOUR_NET_PLAYER2);
		this->createColourChild("NET_PLAYER2_DARK", HUD_COLOUR_NET_PLAYER2_DARK);
		this->createColourChild("NET_PLAYER3", HUD_COLOUR_NET_PLAYER3);
		this->createColourChild("NET_PLAYER3_DARK", HUD_COLOUR_NET_PLAYER3_DARK);
		this->createColourChild("NET_PLAYER4", HUD_COLOUR_NET_PLAYER4);
		this->createColourChild("NET_PLAYER4_DARK", HUD_COLOUR_NET_PLAYER4_DARK);
		this->createColourChild("NET_PLAYER5", HUD_COLOUR_NET_PLAYER5);
		this->createColourChild("NET_PLAYER5_DARK", HUD_COLOUR_NET_PLAYER5_DARK);
		this->createColourChild("NET_PLAYER6", HUD_COLOUR_NET_PLAYER6);
		this->createColourChild("NET_PLAYER6_DARK", HUD_COLOUR_NET_PLAYER6_DARK);
		this->createColourChild("NET_PLAYER7", HUD_COLOUR_NET_PLAYER7);
		this->createColourChild("NET_PLAYER7_DARK", HUD_COLOUR_NET_PLAYER7_DARK);
		this->createColourChild("NET_PLAYER8", HUD_COLOUR_NET_PLAYER8);
		this->createColourChild("NET_PLAYER8_DARK", HUD_COLOUR_NET_PLAYER8_DARK);
		this->createColourChild("NET_PLAYER9", HUD_COLOUR_NET_PLAYER9);
		this->createColourChild("NET_PLAYER9_DARK", HUD_COLOUR_NET_PLAYER9_DARK);
		this->createColourChild("NET_PLAYER10", HUD_COLOUR_NET_PLAYER10);
		this->createColourChild("NET_PLAYER10_DARK", HUD_COLOUR_NET_PLAYER10_DARK);
		this->createColourChild("NET_PLAYER11", HUD_COLOUR_NET_PLAYER11);
		this->createColourChild("NET_PLAYER11_DARK", HUD_COLOUR_NET_PLAYER11_DARK);
		this->createColourChild("NET_PLAYER12", HUD_COLOUR_NET_PLAYER12);
		this->createColourChild("NET_PLAYER12_DARK", HUD_COLOUR_NET_PLAYER12_DARK);
		this->createColourChild("NET_PLAYER13", HUD_COLOUR_NET_PLAYER13);
		this->createColourChild("NET_PLAYER13_DARK", HUD_COLOUR_NET_PLAYER13_DARK);
		this->createColourChild("NET_PLAYER14", HUD_COLOUR_NET_PLAYER14);
		this->createColourChild("NET_PLAYER14_DARK", HUD_COLOUR_NET_PLAYER14_DARK);
		this->createColourChild("NET_PLAYER15", HUD_COLOUR_NET_PLAYER15);
		this->createColourChild("NET_PLAYER15_DARK", HUD_COLOUR_NET_PLAYER15_DARK);
		this->createColourChild("NET_PLAYER16", HUD_COLOUR_NET_PLAYER16);
		this->createColourChild("NET_PLAYER16_DARK", HUD_COLOUR_NET_PLAYER16_DARK);
		this->createColourChild("NET_PLAYER17", HUD_COLOUR_NET_PLAYER17);
		this->createColourChild("NET_PLAYER17_DARK", HUD_COLOUR_NET_PLAYER17_DARK);
		this->createColourChild("NET_PLAYER18", HUD_COLOUR_NET_PLAYER18);
		this->createColourChild("NET_PLAYER18_DARK", HUD_COLOUR_NET_PLAYER18_DARK);
		this->createColourChild("NET_PLAYER19", HUD_COLOUR_NET_PLAYER19);
		this->createColourChild("NET_PLAYER19_DARK", HUD_COLOUR_NET_PLAYER19_DARK);
		this->createColourChild("NET_PLAYER20", HUD_COLOUR_NET_PLAYER20);
		this->createColourChild("NET_PLAYER20_DARK", HUD_COLOUR_NET_PLAYER20_DARK);
		this->createColourChild("NET_PLAYER21", HUD_COLOUR_NET_PLAYER21);
		this->createColourChild("NET_PLAYER21_DARK", HUD_COLOUR_NET_PLAYER21_DARK);
		this->createColourChild("NET_PLAYER22", HUD_COLOUR_NET_PLAYER22);
		this->createColourChild("NET_PLAYER22_DARK", HUD_COLOUR_NET_PLAYER22_DARK);
		this->createColourChild("NET_PLAYER23", HUD_COLOUR_NET_PLAYER23);
		this->createColourChild("NET_PLAYER23_DARK", HUD_COLOUR_NET_PLAYER23_DARK);
		this->createColourChild("NET_PLAYER24", HUD_COLOUR_NET_PLAYER24);
		this->createColourChild("NET_PLAYER24_DARK", HUD_COLOUR_NET_PLAYER24_DARK);
		this->createColourChild("NET_PLAYER25", HUD_COLOUR_NET_PLAYER25);
		this->createColourChild("NET_PLAYER25_DARK", HUD_COLOUR_NET_PLAYER25_DARK);
		this->createColourChild("NET_PLAYER26", HUD_COLOUR_NET_PLAYER26);
		this->createColourChild("NET_PLAYER26_DARK", HUD_COLOUR_NET_PLAYER26_DARK);
		this->createColourChild("NET_PLAYER27", HUD_COLOUR_NET_PLAYER27);
		this->createColourChild("NET_PLAYER27_DARK", HUD_COLOUR_NET_PLAYER27_DARK);
		this->createColourChild("NET_PLAYER28", HUD_COLOUR_NET_PLAYER28);
		this->createColourChild("NET_PLAYER28_DARK", HUD_COLOUR_NET_PLAYER28_DARK);
		this->createColourChild("NET_PLAYER29", HUD_COLOUR_NET_PLAYER29);
		this->createColourChild("NET_PLAYER29_DARK", HUD_COLOUR_NET_PLAYER29_DARK);
		this->createColourChild("NET_PLAYER30", HUD_COLOUR_NET_PLAYER30);
		this->createColourChild("NET_PLAYER30_DARK", HUD_COLOUR_NET_PLAYER30_DARK);
		this->createColourChild("NET_PLAYER31", HUD_COLOUR_NET_PLAYER31);
		this->createColourChild("NET_PLAYER31_DARK", HUD_COLOUR_NET_PLAYER31_DARK);
		this->createColourChild("NET_PLAYER32", HUD_COLOUR_NET_PLAYER32);
		this->createColourChild("NET_PLAYER32_DARK", HUD_COLOUR_NET_PLAYER32_DARK);

		this->createColourChild("PURE_WHITE", HUD_COLOUR_PURE_WHITE);
		this->createColourChild("WHITE", HUD_COLOUR_WHITE);
		this->createColourChild("BLACK", HUD_COLOUR_BLACK);
		this->createColourChild("GREY", HUD_COLOUR_GREY);
		this->createColourChild("GREYLIGHT", HUD_COLOUR_GREYLIGHT);
		this->createColourChild("GREYDARK", HUD_COLOUR_GREYDARK);
		this->createColourChild("RED", HUD_COLOUR_RED);
		this->createColourChild("REDLIGHT", HUD_COLOUR_REDLIGHT);
		this->createColourChild("REDDARK", HUD_COLOUR_REDDARK);
		this->createColourChild("BLUE", HUD_COLOUR_BLUE);
		this->createColourChild("BLUELIGHT", HUD_COLOUR_BLUELIGHT);
		this->createColourChild("BLUEDARK", HUD_COLOUR_BLUEDARK);
		this->createColourChild("YELLOW", HUD_COLOUR_YELLOW);
		this->createColourChild("YELLOWLIGHT", HUD_COLOUR_YELLOWLIGHT);
		this->createColourChild("YELLOWDARK", HUD_COLOUR_YELLOWDARK);
		this->createColourChild("ORANGE", HUD_COLOUR_ORANGE);
		this->createColourChild("ORANGELIGHT", HUD_COLOUR_ORANGELIGHT);
		this->createColourChild("ORANGEDARK", HUD_COLOUR_ORANGEDARK);
		this->createColourChild("GREEN", HUD_COLOUR_GREEN);
		this->createColourChild("GREENLIGHT", HUD_COLOUR_GREENLIGHT);
		this->createColourChild("GREENDARK", HUD_COLOUR_GREENDARK);
		this->createColourChild("PURPLE", HUD_COLOUR_PURPLE);
		this->createColourChild("PURPLELIGHT", HUD_COLOUR_PURPLELIGHT);
		this->createColourChild("PURPLEDARK", HUD_COLOUR_PURPLEDARK);
		this->createColourChild("PINK", HUD_COLOUR_PINK);
	}

	void CommandHudColourEditor::createColourChild(const char* name, int id)
	{
		int r, g, b, a;
		HUD::GET_HUD_COLOUR(id, &r, &g, &b, &a);
		CommandName command_name = utf8ToCmdName(name);
		StringUtils::simplify(command_name);
		this->createChild<CommandHudColourEditorColour>(LIT(name), {
			std::move(CommandName(CMDNAME("hudcolour")).append(command_name)),
			std::move(CommandName(CMDNAME("hudcolor")).append(command_name))
		}, id, r, g, b, a);
	}
}
