#include "CommandListInteriors.hpp"

#include "CommandLambdaListSelect.hpp"
#include "CommandLambdaToggle.hpp"

#include "CommandInteriorBunker.hpp"
#include "CommandInteriorVehware.hpp"

namespace Stand
{
	CommandListInteriors::CommandListInteriors(CommandList* parent)
		: CommandList(parent, LOC("INTS"))
	{
		{
			auto list = this->createChild<CommandList>(LOC("INT_BKR"));

			auto bunker = list->createChild<CommandInteriorBunker>();
			bunker->style = list->createChild<CommandLambdaListSelect>(LOC("STYL"), { CMDNAME("bunkerstyle") }, NOLABEL, [bunker](long long, Click&)
			{
				bunker->onOptionChanged();
			}, std::vector<CommandListActionItemData>{
				{ 0, LIT("1"), CMDNAMES("1") },
				{ 1, LIT("2"), CMDNAMES("2") },
				{ 2, LIT("3"), CMDNAMES("3") },
			}, 1, CMDFLAGS_LIST_SELECT & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
			bunker->security = list->createChild<CommandLambdaToggle>(LOC("SECTY"), { CMDNAME("bunkersecurity") }, [bunker](bool, Click&)
			{
				bunker->onOptionChanged();
			}, true, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
			bunker->equipment_upgrade = list->createChild<CommandLambdaToggle>(LOC("EQUIPUPGR"), { CMDNAME("bunkerequipmentupgrade") }, [bunker](bool, Click&)
			{
				bunker->onOptionChanged();
			}, true, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
			bunker->gun_locker = list->createChild<CommandLambdaToggle>(LOC("GUNLKR"), { CMDNAME("bunkergunlocker") }, [bunker](bool, Click&)
			{
				bunker->onOptionChanged();
			}, true, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
			bunker->personal_quarters = list->createChild<CommandLambdaToggle>(LOC("PSNLQRTS"), { CMDNAME("bunkerpersonalquarters") }, [bunker](bool, Click&)
			{
				bunker->onOptionChanged();
			}, true, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
			bunker->gun_range = list->createChild<CommandLambdaToggle>(LOC("GUNRNGE"), { CMDNAME("bunkergunrange") }, [bunker](bool, Click&)
			{
				bunker->onOptionChanged();
			}, true, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
		}

		{
			auto list = this->createChild<CommandList>(LOC("INT_VW"));

			auto vehware = list->createChild<CommandInteriorVehware>();
			vehware->style = list->createChild<CommandLambdaListSelect>(LOC("STYL"), { CMDNAME("vehwarestyle") }, NOLABEL, [vehware](long long, Click&)
			{
				vehware->onOptionChanged();
			}, std::vector<CommandListActionItemData>{
				{ 2, LOC("INT_VW_2"), CMDNAMES("branded") },
				{ 1, LOC("INT_VW_1"), CMDNAMES("urban") },
				{ 0, LOC("INT_VW_0_1"), CMDNAMES("basic1") },
				{ -1, LOC("INT_VW_0_2"), CMDNAMES("basic2") },
				{ -2, LOC("INT_VW_0_3"), CMDNAMES("basic3") },
				{ -3, LOC("INT_VW_0_4"), CMDNAMES("basic4") },
			}, 1, CMDFLAGS_LIST_SELECT & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS);
		}

		lscm = this->createChild<CommandInterior>(LOC("LSCM"), { CMDNAME("tpcarmeet") }, v3(-2000.0f, 1113.4f, 25.7f), true);
		this->createChild<CommandInterior>(LOC("INT_IF"), { CMDNAME("tpfacility") }, v3(2047.0f, 2942.0f, -61.9f), true);
		server_room = this->createChild<CommandInterior>(LOC("INT_SR"), { CMDNAME("tpserverroom"), CMDNAME("tpserveroom") }, v3(2168.0f, 2920.0f, -84.0f), true);
		this->createChild<CommandInterior>(LOC("INT_BS"), { CMDNAME("tpsubmarine") }, v3(514.19403f, 4838.989f, -62.587917f), true);
		this->createChild<CommandInterior>(LOC("INT_HL"), { CMDNAME("tphumanelabs") }, v3(3616.85, 3738.5544, 28.69009));
		this->createChild<CommandInterior>(LOC("INT_FR"), { CMDNAME("tpburntfib") }, v3(115.09295f, -747.57446f, 234.15239f));
		this->createChild<CommandInterior>(LIT("FIB"), { CMDNAME("tpfib") }, v3(129.75044f, -763.3821f, 242.1519f));
		this->createChild<CommandInterior>(LIT("IAA"), { CMDNAME("tpiaa") }, v3(114.059616f, -619.2508f, 206.04666f));
		ranch = this->createChild<CommandInterior>(LOC("INT_LFB"), { CMDNAME("tpranch") }, v3(1399.973f, 1148.756f, 113.3336f));
		this->createChild<CommandInterior>(LOC("INT_SSWCC"), { CMDNAME("tpcomedy") }, v3(377.253662f, -998.272339f, -98.999947f));
		bahamamamas = this->createChild<CommandInterior>(LOC("INT_BM"), { CMDNAME("tpbahamamamas") }, v3(-1388.0013f, -618.41967f, 30.819599f));
		this->createChild<CommandInterior>(LOC("INT_TR"), { CMDNAME("tptortureroom") }, v3(144.77847f, -2201.925f, 4.68802f));
		this->createChild<CommandInterior>(LOC("INT_MR"), { CMDNAME("tpmotelroom") }, v3(152.2604f, -1004.47107f, -99.00476f));
		this->createChild<CommandInterior>(LOC("INT_CIN"), { CMDNAME("tpcinema") }, v3(-1425.5645f, -244.3f, 16.8053f));
		this->createChild<CommandInterior>(LOC("INT_RCYL"), { CMDNAME("tprecyling") }, v3(-598.6379f, -1608.399f, 26.0108f));
		//this->createChild<CommandInterior>(LOC("INT_SLAU"), v3(982.233f, -2160.382f, 28.4761f), "v_abattoir"); // Might need doors (v_ilev_abbmaindoor, etc.)
		this->createChild<CommandInterior>(LOC("INT_OG"), { CMDNAME("tpomega") }, v3(2330.9282f, 2574.3071f, 46.680347f));
		this->createChild<CommandInterior>(LOC("INT_LH"), { CMDNAME("tplester") }, v3(1273.9f, -1719.305f, 54.77141f));
		therapy = this->createChild<CommandInterior>(LOC("INT_PYSC"), { CMDNAME("tptherapy") }, v3(-1908.024f, -573.4244f, 19.09722f));
		sol = this->createChild<CommandInterior>(LOC("INT_SOL"), { CMDNAME("tpsolomon") }, v3(-1005.663f, -478.3461f, 49.0265f));
		this->createChild<CommandInterior>(LOC("INT_FH"), { CMDNAME("tpfloyd") }, v3(-1152.3326f, -1518.9675f, 10.632729f));
		this->createChild<CommandInterior>(LOC("INT_JH"), { CMDNAME("tpjanitor") }, v3(-111.7116f, -11.912f, 69.5196f));
		benny = this->createChild<CommandInterior>(LOC("INT_BENNY"), { CMDNAME("tpbenny") }, v3(-210.29391, -1324.2723, 30.890385), true);
		chopshop = this->createChild<CommandInterior>(LOC("INT_HAYES"), { CMDNAME("tpchopshop") }, v3(479.0568f, -1316.825f, 28.2038f));
		tequilala = this->createChild<CommandInterior>(LOC("INT_TEQ"), { CMDNAME("tprockclub"), CMDNAME("tptequilala") }, v3(-556.5089f, 286.3181f, 81.1763f));
		this->createChild<CommandInterior>(LOC("INT_FDRY"), { CMDNAME("tpfoundry") }, v3(1100.1342f, -2002.1356f, 30.063913f));
		udg = this->createChild<CommandInterior>(LOC("INT_UDG"), { CMDNAME("tpcarpark") }, v3(-16.29585f, -684.0385f, 33.50832f));
		eps = this->createChild<CommandInterior>(LOC("INT_ESR"), { CMDNAME("tpepsilonstorage") }, v3(245.1564f, 370.211f, 104.7382f));
		this->createChild<CommandInterior>(LOC("INT_CC"), v3(402.5164f, -1002.847f, -99.2587f));
		this->createChild<CommandInterior>(LOC("INT_ME"), v3(405.9228f, -954.1149f, -99.6627f));
		this->createChild<CommandInterior>(LOC("INT_NG"), v3(-1505.783f, -3012.587f, -79.9999f), true);
		this->createChild<CommandInterior>(LOC("INT_WEED"), { CMDNAME("tpweedfarm") }, v3(1042.3301, -3199.142, -38.161537), true);
		this->createChild<CommandInterior>(LOC("INT_STILT"), { CMDNAME("tpstilthouse") }, v3(372.6707f, 405.5235f, 144.5326f), true);
		this->createChild<CommandInterior>(LOC("INT_A_H"), v3(-282.0588f, -955.17f, 85.3036f));
		this->createChild<CommandInterior>(LOC("INT_A_M"), v3(342.7946f, -997.4225f, -99.7444f));
		this->createChild<CommandInterior>(LOC("INT_A_L"), v3(260.3268f, -997.4298f, -100.0086f));
		this->createChild<CommandInterior>(LOC("INT_G_H"), v3(228.6161f, -992.053f, -99.9999f));
		this->createChild<CommandInterior>(LOC("INT_G_M"), v3(199.9716f, -999.6678f, -100.0f));
		this->createChild<CommandInterior>(LOC("INT_G_L"), v3(173.1165f, -1003.28f, -99.9999f));
		this->createChild<CommandInterior>(LOC("INT_W_L"), v3(1010.008f, -3100.0f, -39.9999f), true);
		this->createChild<CommandInterior>(LOC("INT_W_M"), v3(1059.995f, -3100.0f, -39.9999f), true);
		this->createChild<CommandInterior>(LOC("INT_W_S"), v3(1094.997f, -3100.012f, -39.9999f), true);
	}
}
