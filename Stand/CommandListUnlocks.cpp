#include "CommandListUnlocks.hpp"

#include <memory>

#include "CommandLambdaAction.hpp"
#include "CommandLambdaActionScript.hpp"

#include "lang.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "ScriptGlobal.hpp"
#include "StatUtil.hpp"
#include "Util.hpp"

namespace Stand
{
	struct Unlock
	{
		const Label name;
		const std::function<void(const Click&)> impl;
		const Label help_text = NOLABEL;
		const std::vector<CommandName> command_names = {};
	};

	static constexpr int casino_heist_clothing_unlocks[] = {
		28225, // Refuse Collectors
		28226, // Undertakers
		28227, // Valet Outfits
		28229, // Prision Guards
		28230, // FIB Suits
		28232, // Gruppe Sechs Gear
		28233, // Bugstars Uniforms
		28234, // Maintenance
		28235, // Yung Ancestors
		28236, // Firefighter Gear
		28237, // Orderly Armor
		28238, // Upscale Armor
		28239, // Evening Armor
		28240, // Reinforced: Padded Combat
		28241, // Reinforced: Bulk Combat
		28242, // Reinforced: Compact Combat
		28243, // Balaclava Crook
		28244, // Classic Crook
		28245, // High-end Crook
		28246, // Infiltration: Upgraded Tech
		28247, // Infiltration: Advanced Tech
		28248, // Infiltration: Modernized Tech
	};

	static constexpr int bunker_research_unlocks[] = {
		15381, // APC Sam Battery
		15382, // Ballistic Equipment
		15428, // Half-track 20mm Quad Autocannon
		15429, // Weaponized Tampa Dual Remote Minigun
		15430, // Weaponized Tampa Rear-Firing Mortar
		15431, // Weaponized Tampa Front Missile Launchers
		15491, // Weaponized Tampa Heavy Chassis Upgrade
		15432, // Dune FAV 40mm Grenade Launcher
		15433, // Dune FAV 7.62mm Minigun
		15434, // Insurgent Pick-Up Custom .50 Cal Minigun
		15435, // Insurgent Pick-Up Custom Heavy Armor Plating
		15436, // Technical Custom 7.62mm Minigun
		15437, // Technical Custom Ram-bar
		15438, // Technical Custom Brute-bar
		15439, // Technical Custom Heavy Chassis Upgrade
		15447, // Oppressor Missile Launchers
		15448, // Fractal Livery Set
		15449, // Digital Livery Set
		15450, // Geometric Livery Set
		15451, // Nature Reserve Livery Set
		15452, // Naval Battle Livery Set
		15453, // Anti-Aircraft Trailer Dual 20mm Flak
		15454, // Anti-Aircraft Trailer Homing Missile Battery
		15455, // Mobile Operations Center Rear Turrets
		15456, // Incendiary Rounds
		15457, // Hollow Point Rounds
		15458, // Armor Piercing Rounds
		15459, // Full Metal Jacket Rounds
		15460, // Explosive Rounds
		15461, // Pistol Mk II Mounted Scope
		15462, // Pistol Mk II Compensator
		15463, // SMG Mk II Holographic Sight
		15464, // SMG Mk II Heavy Barrel
		15465, // Heavy Sniper Mk II Night Vision Scope
		15466, // Heavy Sniper Mk II Thermal Scope
		15467, // Heavy Sniper Mk II Heavy Barrel
		15468, // Combat MG Mk II Holographic Sight
		15469, // Combat MG Mk II Heavy Barrel
		15470, // Assault Rifle Mk II Holographic Sight
		15471, // Assault Rifle Mk II Heavy Barrel
		15472, // Carbine Rifle Mk II Holographic Sight
		15473, // Carbine Rifle Mk II Heavy Barrel
		15474, // Proximity Mines
		15492, // Brushstroke Camo Mk II Weapon Livery
		15493, // Skull Mk II Weapon Livery
		15494, // Sessanta Nove Mk II Weapon Livery
		15495, // Perseus Mk II Weapon Livery
		15496, // Leopard Mk II Weapon Livery
		15497, // Zebra Mk II Weapon Livery
		15498, // Geometric Mk II Weapon Livery
		15499, // Boom! Mk II Weapon Livery
	};

	static constexpr int arena_war_clothing_unlocks[] = {
		25244, // Yellow/Blue Robot Bodysuit
		25245, // Blue/Red Hero Bodysuit
		25246, // Green/Purple Shapes Bodysuit
		25247, // Blue&Pink/Red&Green Contours Bodysuit
		25248, // Green/Purple Martian Bodysuit
		25249, // Blue/Red Reptile Bodysuit
		25250, // Blue/Pink Galaxy Bodysuit
		25000, // Red&Blue/Blue&Green Nebula Bodysuit
		25251, // All Space Creature Suits
		25252, // All Space Cyclops Suits
		25253, // All Space Horro Suits
		25254, // All Retro Suits
		25255, // All Astronaut Suits
		25256, // All Space Traveler Suits
		25257, // Pogo Space Monkey
		25258, // Republican Space Ranger
		25265, // Green/Yellow/White/Black Space Rangers Tee
		25266, // Green/Black Space Ranger Logo Tee
		25267, // Yellow/White Phases Tee
		25268, // Pink/Blue Rocket Splash Tee
		24977, // Pink/Black Spacesuit Alien Tee
		25269, // Pink/Blue/Purple Moons Tee
		25270, // Red/Green/Blue Isn't Free Tee 
		25271, // All Raider Tops
		25272, // All Leather Feather Tops
		25273, // All Mercenary Tops
		25274, // Benedict Light Beer Hoodie
		25275, // Taco Bomb Hoodie
		25276, // Cluckin'Bell Logo Bomb Hoodie
		25277, // Patriot Beer Hoodie
		25278, // Pisswasser Hoodie
		25279, // Burger Shot Hoodie
		25280, // Corn Dog Hoodie
		25281, // Donut Hoodie
		25282, // Lucky Plucker Hoodie
		25283, // Logger Light Hoodie
		25284, // Pizza Hoodie
		25285, // Fries Hoodie
		25286, // Mushrooms Hoodie
		25287, // Redwood Hoodie
		25288, // eCola Infectious Hoodie
		25289, // Cluckin'Bell Logo Hoodie
		25290, // Lemons Hoodie
		25291, // Tacos Hoodie
		25292, // Burger Shot Pattern Sweater
		25293, // Burger Shot Logo Sweater
		25294, // Burger Shot Sweater
		25295, // Green/White Sprunk Sweater
		25296, // Wigwam Sweater
		25297, // Taco Bomb Chili Sweater
		25298, // Green/Yellow Bomb Sweater
		25299, // Cluckin'Bell Logo Bomb Sweater
		25300, // Blue Cluckin'Bell Sweater
		25301, // Black Cluckin'Bell Sweater
		25302, // Infectious/Red eCola Sweater
		25303, // Orange/Red MeTV Sweater
		25304, // Magenta/Cyan Heat Sweater
		25305, // Degenatron Sweater
		25306, // Red/Black Pisswasser Sweater
		25307, // Bolt Burger Sweater
		25308, // Lucky Plucker Logo Bomb Sweater
		25309, // Lucky Plucker Sweater
		25310, // Dark Red/Red/Black/Blue Burger Shot Hockey Shirt
		25311, // Black/Blue Cluckin' Bell Hockey Shirt
		25312, // Wigwam Hockey Shirt
		25313, // Redwood Hockey Shirt
		25314, // Bean Machine Hockey Shirt
		25315, // Red eCola Hockey Shirt
		25316, // Black eCola Hockey Shirt
		25317, // Blue/Black Phat Chips Hockey Shirt
		25318, // Green/Dark Green Sprunk Hockey Shirt
		25319, // Sprunk Classic Hockey Shirt
		25320, // Burger Shot Black Tee
		25321, // Burger Shot Logo Tee
		25322, // Cluckin'Bell Logo Tee
		25323, // Cluckin'Bell Black Tee
		25324, // Cluckin'Bell Filled Logo Tee
		25325, // eCola Black Tee
		25326, // Lucky Plucker Tee
		25327, // Pisswasser Tee
		25328, // Sprunk Tee
		25329, // Taco Bomb Chili Tee
		25330, // Taco Bomb Black Tee
		25331, // Up-n-Atom Hamburgers Tee
		25332, // Up-n-Atom Tee
		25333, // Wigwam Tee
		25334, // Degenatron ROYGBIV Tee
		25335, // CNT Tee
		25336, // Qub3d Tee
		25337, // Righteous Slaughter Tee
		25338, // Space Monkey Full Tee
		25339, // Space Monkey Pixel Tee
		25340, // Space Monkey Enemy Tee
		25341, // Burger Shot Tee
		25342, // Heat Rises Tee
		25343, // Space Monkey Logo Tee
		25344, // Space Monkey Suit Tee
		25345, // Space Monkey Face Tee
		25346, // Space Monkey Mosaic Tee
		25347, // Bolt Burger Logo Tee
		24970, // Bolt Burger Hunger Tee
		25348, // Exsorbeo 720 Tee
		25349, // Heat Blue Logo Tee
		25350, // Heat Blue Ball Logo Tee
		25351, // Heat Blue Pop Art Logo Tee
		25352, // MeTV Blue 90s Tee
		25353, // MeTV Blue Safari Tee
		25354, // Burger Shot Target Tee
		25355, // eCola Infectious Tee
		25356, // Up-n-Atom White Tee
		25357, // Jock Cranley Patriot Tee
		25358, // CCC TV Tee
		25359, // Degenatron Logo Tee
		25360, // eCola White Tee
		25361, // eCola Pass It On Tee
		25362, // TW@ Tee
		25363, // All Chain Pants
		25364, // All Chain Shorts
		25365, // All Stitch Pants
		25366, // All Raider Pants // Raider Shoes+Pants
		25366, // All Raider Boots // Raider Shoes+Pants
		25367, // All Light Ups
		25368, // All Flaming Skull Boots
		25369, // All Skull Harness Boots
		25370, // All Plated Boots
		25371, // Burger Shot Fries/Burgers Cap
		25373, // Burger Shot Logo Cap
		25374, // Burger Shot Bullseye Cap
		25375, // Yellow/Blue Cluckin'Bell Cap
		25377, // Cluckin'Bell Logos Cap
		25378, // Pink/Purple/Black Hotdogs Cap
		25379, // Green/White/- Taco Bomb Cap
		25382, // Red/White Lucky Plucker Cap
		25383, // Lucky Plucker White/Red Pattern Cap
		25386, // Black/White Pisswasser Cap
		25390, // Taco Canvas Hat
		25391, // Burger Shot Canvas Hat
		25392, // Cluckin'Bell Canvas Hat
		25393, // Hotdogs Canvas Hat
	};

	CommandListUnlocks::CommandListUnlocks(CommandList* const parent)
		: CommandList(parent, LOC("UNLKS"), { CMDNAME("unlocks") })
	{
		static Unlock unlocks[] = {
			{
				LOC("FREEORB"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						StatUtil::setPackedBool(PACKED_MP_BOOL_AWARD_XMAS2017_CANNON_GIFT, TRUE, char_slot);
						StatUtil::setPackedBool(PACKED_MP_BOOL_PURCHASE_XMAS2017_CANNON_GIFT, FALSE, char_slot);
					}
				},
				NOLABEL,
				CMDNAMES_OBF("freeorbital"),
			},
			{
				LOC("GNDRCHNG"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "ALLOW_GENDER_CHANGE"), 1, true);
						if (click.type != CLICK_AUTO)
						{
							click.setResponse(LOC("GNDRCHNG_T"));
						}
					}
				},
				LOC("GNDRCHNG_H"),
				CMDNAMES("unlockgenderchange", "unlocksexchange")
			},
			{
				LOC("LVRIES"),
				[](const Click& click)
				{
					if (click.inOnline())
					{
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES0"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES1"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES2"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES3"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES4"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES5"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES6"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES7"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES8"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES9"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES10"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES11"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES12"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES13"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES14"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES15"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES16"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES17"), -1, true);
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMASLIVERIES18"), -1, true);
						*ScriptGlobal(GLOBAL_ATOMIZER_XMAS_TINT).as<int*>() = 90;
						if (click.type != CLICK_AUTO)
						{
							click.setResponse(LOC("LVRIES_T"));
						}
					}
				},
				NOLABEL,
				CMDNAMES("unlockliveries", "unlockexclusiveliveries")
			},
			{
				LOC("RSRCH"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						for (const int& i : bunker_research_unlocks)
						{
							StatUtil::setPackedBool(i, TRUE, char_slot);
						}
						if (click.type != CLICK_AUTO)
						{
							click.setResponse(LOC("RSRCH_T"));
						}
					}
				},
				NOLABEL,
				CMDNAMES("unlockresearch", "unlockbunkerresearch")
			},
			{
				LOC("UNLKOSA"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						StatUtil::setPackedBool(30309, TRUE, char_slot);
					}
				},
				NOLABEL,
				CMDNAMES("unlockkosatka")
			},
			{
				LOC("H3CLTH"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						for (const int& i : casino_heist_clothing_unlocks)
						{
							StatUtil::setPackedBool(i, TRUE, char_slot);
						}
						if (click.type != CLICK_AUTO)
						{
							click.setResponse(LOC("CLTH_T"));
						}
					}
				},
				NOLABEL,
				CMDNAMES("unlockcasinoheistclothing")
			},
			{
				LOC("AWCLTH"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						for (const int& i : arena_war_clothing_unlocks)
						{
							StatUtil::setPackedBool(i, TRUE, char_slot);
						}
						if (click.type != CLICK_AUTO)
						{
							click.setResponse(LOC("CLTH_T"));
						}
					}
				},
				NOLABEL,
				CMDNAMES("unlockarenawarclothing")
			},
			{
				LOC("UNLKSR"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "SR_TIER_1_REWARD"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "SR_INCREASE_THROW_CAP"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "SR_TIER_3_REWARD"), true, true);
						if (click.type != CLICK_AUTO)
						{
							click.setResponse(LOC("UNLKSR_T"));
						}
					}
				},
				LOC("UNLKSR_H"),
				CMDNAMES("unlockshootingrangerewards", "unlockbunkershootingrangerewards")
			},
			{
				LOC("AWDS"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_BEGINNER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_FIELD_FILLER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_ARMCHAIR_RACER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_LEARNER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_SUNDAY_DRIVER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_THE_ROOKIE"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_BUMP_AND_RUN"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_GEAR_HEAD"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_DOOR_SLAMMER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_HOT_LAP"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_ARENA_AMATEUR"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_PAINT_TRADER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_SHUNTER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_JOCK"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_WARRIOR"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_T_BONE"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_MAYHEM"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_WRECKER"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_CRASH_COURSE"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_ARENA_LEGEND"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_PEGASUS"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_CONTACT_SPORT"), true, true);
						STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "AWD_UNSTOPPABLE"), true, true);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_WATCH_YOUR_STEP"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_TOWER_OFFENSE"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_READY_FOR_WAR"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_THROUGH_A_LENS"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_SPINNER"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_YOUMEANBOOBYTRAPS"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_MASTER_BANDITO"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_SITTING_DUCK"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_CROWDPARTICIPATION"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_KILL_OR_BE_KILLED"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_MASSIVE_SHUNT"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_YOURE_OUTTA_HERE"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_WEVE_GOT_ONE"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_ARENA_WAGEWORKER"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_TIME_SERVED"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_TOP_SCORE"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_CAREER_WINNER"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "LAP_DANCED_BOUGHT"), 25);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_MENTALSTATE_TO_NORMAL"), 25);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_TRADE_IN_YOUR_PROPERTY"), 25);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_CONTROL_CROWDS"), 25);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_100_KILLS_PISTOL"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_100_KILLS_SNIPER"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_50_KILLS_GRENADES"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_100_KILLS_SHOTGUN"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_100_KILLS_SMG"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_50_KILLS_ROCKETLAUNCH"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_25_KILLS_STICKYBOMBS"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_20_KILLS_MELEE"), 20);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_100_HEADSHOTS"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_50_VEHICLES_BLOWNUP"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_ENEMYDRIVEBYKILLS"), 10);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_VEHICLES_JACKEDR"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_HOLD_UP_SHOPS"), 19);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_COPS_KILLED"), 200);
					}
				},
				LOC("AWDS_H"),
				CMDNAMES("unlockawards")
			},
			{
				LOC("VEHCUST"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "CHAR_FM_CARMOD_1_UNLCK"), -1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "CHAR_FM_CARMOD_2_UNLCK"), -1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "CHAR_FM_CARMOD_3_UNLCK"), -1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "CHAR_FM_CARMOD_4_UNLCK"), -1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "CHAR_FM_CARMOD_5_UNLCK"), -1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "CHAR_FM_CARMOD_6_UNLCK"), -1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "CHAR_FM_CARMOD_7_UNLCK"), -1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_FMRALLYWONDRIVE"), 1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_FMRALLYWONNAV"), 1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_FMWINSEARACE"), 1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_FMWINAIRRACE"), 1);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "NUMBER_TURBO_STARTS_IN_RACE"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "USJS_COMPLETED"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "RACES_WON"), 50);
						Util::STAT_SET_INT_IF_LESS(ATSTRINGHASH("MPPLY_TOTAL_RACES_WON"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_FM_RACES_FASTEST_LAP"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "NUMBER_SLIPSTREAMS_IN_RACE"), 100);

						// Chrome Rims
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_WIN_CAPTURES"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_DROPOFF_CAP_PACKAGES"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_KILL_CARRIER_CAPTURE"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_FINISH_HEISTS"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_FINISH_HEIST_SETUP_JOB"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_NIGHTVISION_KILLS"), 100);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_WIN_LAST_TEAM_STANDINGS"), 50);
						Util::STAT_SET_INT_IF_LESS(CHAR_STAT(char_slot, "AWD_ONLY_PLAYER_ALIVE_LTS"), 50);

						// Unlock eCola and Sprunk plate types in LS Customs
						STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_XMAS23_PLATES0"), -1, TRUE);
					}
				},
				NOLABEL,
				CMDNAMES("unlockvehiclecustomisations", "unlockvehiclecustomizations")
			},
			{
				LOC("UNLKAMUTINTS"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "PISTOL_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CMBTPISTOL_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "APPISTOL_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "MICROSMG_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SMG_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "ASLTRIFLE_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CRBNRIFLE_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "ADVRIFLE_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "MG_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CMBTMG_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "ASLTSMG_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "PUMP_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "ASLTSHTGN_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SNIPERRFL_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "HVYSNIPER_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "GRNLAUNCH_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "RPG_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "MINIGUNS_ENEMY_KILLS"), 600, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SAWNOFF_ENEMY_KILLS"), 600, true);
					}
				},
				LOC("UNLKAMUTINTS_H"),
				CMDNAMES("unlockweapontints")
			},
			{
				LOC("FLLSTATS"),
				[](const Click& click)
				{
					const int char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SCRIPT_INCREASE_STAM"), 100, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SCRIPT_INCREASE_STRN"), 100, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SCRIPT_INCREASE_LUNG"), 100, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SCRIPT_INCREASE_DRIV"), 100, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SCRIPT_INCREASE_FLY"), 100, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SCRIPT_INCREASE_SHO"), 100, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "SCRIPT_INCREASE_STL"), 100, true);
					}
				},
				LOC("FLLSTATS_H"),
				CMDNAMES("unlockcharacterstats")
			},
		};
		CommandPhysical::collapse_command_names.emplace(L"unlock");
		this->createChild<CommandLambdaAction>(LOC("UNLKALL"), CMDNAMES("unlockall"), LOC("UNLKALL_H"), [](Click& click)
		{
			if (click.inOnline())
			{
				for (const Unlock& unlock : unlocks)
				{
					unlock.impl(click.deriveAuto());
				}
			}
		});
		for (const Unlock& unlock : unlocks)
		{
			this->createChild<CommandLambdaActionScript>(Label(unlock.name), std::vector<CommandName>(unlock.command_names), Label(unlock.help_text), std::function<void(const Click&)>(unlock.impl));
		}
	}
}
