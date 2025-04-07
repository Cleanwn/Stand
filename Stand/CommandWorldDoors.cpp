#include "CommandWorldDoors.hpp"

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "BlipUtil.hpp"
#include "CommandInteriorBunker.hpp"
#include "CommandInteriorVehware.hpp"
#include "CommandListInteriors.hpp"
#include "tbDeceiveInteriorChecks.hpp"
#include "tbDisableDeathbarriers.hpp"
#include "FiberPool.hpp"
#include "force_door_open.hpp"
#include "get_current_time_millis.hpp"
#include "gta_input.hpp"
#include "HudColour.hpp"
#include "input.hpp"
#include "is_session.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "ToggleableBehaviourIpl.hpp"
#include "Util.hpp"
#include "WorldState.hpp"

#define DOORS_DEV_MODE false

#ifndef STAND_DEBUG
static_assert(!DOORS_DEV_MODE);
#endif

#if DOORS_DEV_MODE
#include "drawDebugText.hpp"
#include "DrawUtil3d.hpp"
#include "Renderer.hpp"
#endif

namespace Stand
{
	class MarkerInterior : public Marker
	{
	private:
		CommandInterior const* const* interior;

	public:
		explicit MarkerInterior(v3&& pos, float&& heading, Label&& name, CommandInterior const* const* const interior, Flags&& flags = NONE) noexcept
			: Marker(std::move(pos), std::move(heading), std::move(name), std::move(flags)), interior(interior)
		{
		}

		void onEnter() const final
		{
			g_tb_disable_deathbarriers.enable();
			(*interior)->enable();
		}

		void onLeave() const final
		{
			g_tb_disable_deathbarriers.disable();
		}
	};

	class MarkerApartment : public Marker
	{
	private:
		const std::vector<hash_t> exterior_cull_models;

	public:
		explicit MarkerApartment(v3&& pos, float&& heading, Label&& name, std::vector<hash_t>&& exterior_cull_models) noexcept
			: Marker(std::move(pos), std::move(heading), std::move(name), Marker::SP_ONLY), exterior_cull_models(std::move(exterior_cull_models))
		{
		}

		void onTick() const final
		{
			for (const auto& exterior_cull_model : exterior_cull_models)
			{
				INTERIOR::ENABLE_EXTERIOR_CULL_MODEL_THIS_FRAME(exterior_cull_model);
			}
		}
	};

	class MarkerStilthouse : public MarkerApartment
	{
	private:
		const char* const ipl;

	public:
		explicit MarkerStilthouse(v3&& pos, float&& heading, Label&& name, const char* const ipl, std::vector<hash_t>&& exterior_cull_models) noexcept
			: MarkerApartment(std::move(pos), std::move(heading), std::move(name), std::move(exterior_cull_models)), ipl(ipl)
		{
		}

		void onEnter() const final
		{
			WorldState::setOnline(TRUE);
			STREAMING::REQUEST_IPL(ipl);
		}

		void onLeave() const final
		{
			STREAMING::REMOVE_IPL(ipl);
		}
	};

	class MarkerBunker : public Marker
	{
	public:
		explicit MarkerBunker()
			: Marker({ 894.70526, -3245.9607, -98.25847 }, 90.0f, LOC("INT_BKR"), Marker::SP_ONLY)
		{
		}

		void onEnter() const final
		{
			CommandInteriorBunker::instance->loadFreshRandomised();
		}
	};

	class MarkerVehware : public Marker
	{
	public:
		explicit MarkerVehware()
			: Marker({ 970.86816, -2987.3152, -39.64699 }, 180.0f, LOC("INT_VW"), Marker::SP_ONLY)
		{
		}

		void onEnter() const final
		{
			CommandInteriorVehware::instance->loadFreshRandomised();
		}
	};

	class MarkerIpl : public Marker
	{
	public:
		ToggleableBehaviourIpl* const ipl;

		explicit MarkerIpl(v3&& pos, float&& heading, Label&& name, ToggleableBehaviourIpl* const ipl) noexcept
			: Marker(std::move(pos), std::move(heading), std::move(name)), ipl(ipl)
		{
		}

		void onEnter() const final
		{
			ipl->enable();
		}

		void onLeave() const final
		{
			ipl->disable();
		}
	};

	static auto bliponly_jewel = MarkerBliponly{ { -632.06757, -238.15074, 38.076836 } };
	static auto bliponly_max_renda_1 = MarkerBliponly{ { -584.03186, -291.6176, 35.094944 } };
	static auto bliponly_max_renda_2 = MarkerBliponly{ { -580.54663, -280.66177, 35.314068 } };
	static auto bliponly_eps = MarkerBliponly{ {242.0228, 360.53632, 105.733795} };
	static auto bliponly_janitors = MarkerBliponly{ {-107.09321, -8.373594, 70.52472} };
	static auto bliponly_psb = MarkerBliponly{ {231.8401, 215.23395, 106.28019} };
	static auto bliponly_torture = MarkerBliponly{ {134.20403, -2203.5354, 7.186539} };
	static auto bliponly_fameorshame = MarkerBliponly{ {-254.63133, -2027.0636, 29.94654} };
	static auto bliponly_hospital = MarkerBliponly{ {298.97253, -584.79126, 43.26084} };
	static auto bliponly_vagos = MarkerBliponly{ {-1104.7883, -1637.6951, 4.6159596} };
	static auto bliponly_noose_storage = MarkerBliponly{ {2513.0154, -318.13214, 92.9994} };
	static auto bliponly_omega = MarkerBliponly{ {2331.638, 2576.2202, 46.66769} };
	static auto bliponly_lifeinvader = MarkerBliponly{ {-1082.4508, -259.72305, 37.785225} };

	static auto bliponly_garage_1 = MarkerBliponly{ {-1354.984, -753.3183, 22.314102} };
	static auto bliponly_garage_2 = MarkerBliponly{ {134.143, -1054.9728, 29.192371} };
	static auto bliponly_garage_3 = MarkerBliponly{ {950.80365, -1698.1649, 30.085114} };

	static auto bliponly_fib_lobby = MarkerBliponly{ {105.58902, -744.4702, 45.754738} };
	static auto bliponly_lesters = MarkerBliponly{ {1274.6674, -1720.8967, 54.68079} };
	static auto bliponly_fleeca_1 = MarkerBliponly{ {315.32654, -275.6637, 53.924805} };
	static auto bliponly_fleeca_2 = MarkerBliponly{ {-2965.8508, 482.98566, 15.697028} };
	static auto bliponly_fleeca_3 = MarkerBliponly{ {-349.76382, -46.553234, 49.03683} };
	static auto bliponly_udg_1 = MarkerBliponly{ {-73.58169, -682.04456, 33.681435} };
	static auto bliponly_udg_2 = MarkerBliponly{ {25.762234, -664.1427, 31.628643} };

	static auto bliponly_garage_sp_1 = MarkerBliponly{ {-1074.4863, -1676.3191, 4.5787973} };
	static auto bliponly_garage_sp_2 = MarkerBliponly{ {1204.4677, -3110.2458, 5.5280185} };

	static auto ext_carclub = MarkerInterior{ {-2220.6648, 1160.5447, 29.77061}, 180.0f, LOC("LSCM"), &CommandListInteriors::lscm, Marker::SP_ONLY };
	static auto ent_carclub = Marker{ {779.00214, -1867.5985, 29.295477}, -100.0f, &ext_carclub, Marker::SP_ONLY };
	static auto ext_carclub_track = Marker{ {-2132.1184, 1106.0375, 25.66231}, -86.0f, LOC("LSCM") };
	static auto ent_carclub_track = Marker{ {-2149.1453, 1106.037, 28.662169}, 96.0f, &ext_carclub_track, Marker::NO_BLIP };
	static auto ext_server_room = MarkerInterior{ {2154.789, 2920.9937, -81.07548}, -90.0f, LOC("INT_SR"), &CommandListInteriors::server_room };
	static auto ent_server_room = Marker{ {2474.0398, -332.60037, 92.99268}, 10.0f, &ext_server_room };
	static auto ext_ranch = MarkerInterior{ {1397.0211, 1141.81, 114.33359}, -87.0f, LOC("INT_LFB"), &CommandListInteriors::ranch };
	static auto ent_ranch = Marker{ {1395.225, 1141.7489, 114.63335}, 95.0f, &ext_ranch };
	static auto ext_bahamamamas = MarkerInterior{ {-1387.7535, -587.6849, 30.319506}, -142.0f, LOC("INT_BM"), &CommandListInteriors::bahamamamas, Marker::PROXIMITY_IS_CONTEXT };
	static auto ent_bahamamamas = Marker{ {-1388.4806, -586.75726, 30.218636}, 34.0f, &ext_bahamamamas };
	static auto ext_therapy = MarkerInterior{ {-1902.207, -572.4197, 19.097233}, 150.0f, LOC("INT_PYSC"), &CommandListInteriors::therapy };
	static auto ent_therapy = Marker{ {-1913.872, -574.57324, 11.435141}, 150.0f, &ext_therapy };
	static auto sol_ext = MarkerInterior{ {-1003.0192, -477.78375, 50.027122}, 117.0f, LOC("INT_SOL"), &CommandListInteriors::sol };
	static auto sol_ent_1 = Marker{ {-1011.1541, -479.85855, 39.970623}, 122.0f, &sol_ext };
	static auto sol_ent_2 = Marker{ {-1007.18646, -486.62842, 39.970345}, 122.0f, &sol_ext };
	static auto ext_benny = MarkerInterior{ {-205.37694, -1315.4332, 30.8904022}, 180.0f, LIT("Benny's Original Motor Works"), &CommandListInteriors::benny };
	static auto ent_benny = Marker{ {-205.91118, -1310.0565, 31.295961}, 0.0f, &ext_benny };
	static auto ext_chopshop = MarkerInterior{ {483.77954, -1315.965, 29.200289}, 118.0f, LIT("Hayes Autos"), &CommandListInteriors::chopshop, Marker::PROXIMITY_IS_CONTEXT };
	static auto ent_chopshop = Marker{ {485.08405, -1315.2189, 29.20865}, -62.0f, &ext_chopshop, Marker::PROXIMITY_IS_CONTEXT };
	static auto ext_tequilala = MarkerInterior{ { -564.4939, 277.78168, 83.13633 }, 270.0f, LIT("Tequi-La-la"), &CommandListInteriors::tequilala };
	static auto ent_tequilala = Marker{ { -564.48846, 275.64615, 83.10399 }, 180.0f, &ext_tequilala };
	static auto ext_comedy = Marker{ {  382.33365, -1001.6021, -99.00011 }, 90.0f, LOC("INT_SSWCC") };
	static auto ent_comedy = Marker{ { -430.04214, 261.3588, 83.006424 }, 180.0f, &ext_comedy };
	static auto ext_finbank = Marker{ {0.95314413, -702.9296, 16.131021}, -10.0f, LIT("Union Depository") };
	static auto ent_finbank = Marker{ {10.215674, -667.76587, 33.449127}, 10.0f, &ext_finbank };
	static auto ext_fib = Marker{ {136.43266, -760.99554, 242.15198}, 160.0f, LIT("FIB") };
	static auto ent_fib = Marker{ {136.43266, -760.99554, 45.752052}, 167.0f, &ext_fib };
	static auto ext_morgue = MarkerIpl{ {275.74698, -1361.3822, 24.5378}, 50.0f, LOC("IPL_MORGUE"), &g_ipl_morgue };
	static auto ent_morgue = Marker{ {240.9635, -1379.0055, 33.741726}, 140.0f, &ext_morgue };
	static auto spc_lost_mc_clubhouse = Marker{ {982.1348, -103.24419, 74.84875}, 0.0f, reinterpret_cast<Marker*>(1), Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE | Marker::NO_MARKER };
	static auto ext_humane = Marker{ {3627.2808, 3746.076, 28.690092}, 146.0f, LOC("INT_HL") };
	static auto ent_humane = Marker{ {3621.3074, 3752.5742, 28.630383}, -30.0f, &ext_humane };

	static auto ext_stilthouse_1 = MarkerStilthouse{ { 341.6694, 437.5529, 149.39404 }, 110.0f, LOC("INT_STILT"), "apa_stilt_ch2_04_ext1", { ATSTRINGHASH("apa_ch2_04_house02"), ATSTRINGHASH("apa_ch2_04_house02_d"), ATSTRINGHASH("apa_ch2_04_M_a_LOD"), ATSTRINGHASH("ch2_04_house02_railings"), ATSTRINGHASH("ch2_04_emissive_04"), ATSTRINGHASH("ch2_04_emissive_04_LOD"), ATSTRINGHASH("apa_ch2_04_house02_details") } };
	static auto ent_stilthouse_1 = Marker{ { 346.68378, 440.83838, 147.70233 }, 290.0f, &ext_stilthouse_1, Marker::SP_ONLY };

	static auto bunker_ext = MarkerBunker{};
	static auto bunker_ent_1 = Marker{ { -3032.0483, 3334.444, 10.22558 }, -70.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_2 = Marker{ { 39.539333, 2930.975, 55.836376 }, -139.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_3 = Marker{ { 492.38495, 3013.8906, 40.94527 }, -21.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_4 = Marker{ { 849.24603, 3021.332, 41.318073 }, 0.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_5 = Marker{ { 2110.005, 3325.6926, 45.353672 }, 136.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_6 = Marker{ { 2489.164, 3161.929, 48.985325 }, 40.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_7 = Marker{ { 1801.4082, 4705.035, 39.876324 }, 105.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_8 = Marker{ { -756.44855, 5943.489, 19.940222 }, -48.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_9 = Marker{ { -3158.4153, 1376.4315, 16.717169 }, -70.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_10 = Marker{ { 1572.5353, 2226.8162, 78.23683 }, 180.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };
	static auto bunker_ent_11 = Marker{ { -389.16873, 4341.087, 56.136383 }, -174.0f, &bunker_ext, Marker::SP_ONLY | Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE };

	static auto vehware_ext = MarkerVehware{};
	static auto vehware_ent_1 = Marker{ { 804.6517, -2219.4731, 29.420275 }, 170.0f, &vehware_ext, Marker::SP_ONLY };
	static auto vehware_ent_2 = Marker{ { -72.56631, -1820.5958, 26.942152 }, -130.0f, &vehware_ext, Marker::SP_ONLY };
	static auto vehware_ent_3 = Marker{ { 1757.9573, -1646.7076, 112.64249 }, -170.0f, &vehware_ext, Marker::SP_ONLY };
	static auto vehware_ent_4 = Marker{ { 144.34967, -3006.2542, 7.030922 }, 0.0f, &vehware_ext, Marker::SP_ONLY };
	static auto vehware_ent_5 = Marker{ { 1007.5439, -1854.4471, 31.039825 }, 170.0f, &vehware_ext, Marker::SP_ONLY };
	static auto vehware_ent_6 = Marker{ { -631.84314, -1778.9623, 23.972662 }, 120.0f, &vehware_ext, Marker::SP_ONLY };
	static auto vehware_ent_7 = Marker{ { -1151.8207, -2170.4368, 13.271458 }, 130.0f, &vehware_ext, Marker::SP_ONLY };
	static auto vehware_ent_8 = Marker{ { -514.7716, -2202.6174, 6.3940215 }, -40.0f, &vehware_ext, Marker::SP_ONLY };
	static auto vehware_ent_9 = Marker{ { 1213.9515, -1262.769, 35.22673 }, 90.0f, &vehware_ext, Marker::SP_ONLY };

	static auto motel_ext = Marker{ {151.40268, -1007.81415, -98.99998}, 0.0f, LOC("INT_MR")};
	static auto motel_ent_1 = Marker{ {566.3188, -1778.2246, 29.353159}, -26.0f, &motel_ext };
	static auto motel_ent_2 = Marker{ {550.2979, -1775.7576, 29.312124}, -115.0f, &motel_ext };
	static auto motel_ent_3 = Marker{ {552.2898, -1771.5514, 29.312098}, -115.0f, &motel_ext };
	static auto motel_ent_4 = Marker{ {554.73364, -1766.4291, 29.312199}, -115.0f, &motel_ext };
	static auto motel_ent_5 = Marker{ {557.997, -1759.8203, 29.313643}, -115.0f, &motel_ext };
	static auto motel_ent_6 = Marker{ {561.47504, -1752.0045, 29.279654}, -115.0f, &motel_ext };
	static auto motel_ent_7 = Marker{ {560.3833, -1777.2522, 33.44705}, 62.0f, &motel_ext };
	static auto motel_ent_8 = Marker{ {559.0805, -1777.5227, 33.444}, -29.0f, &motel_ext };
	static auto motel_ent_9 = Marker{ {549.95605, -1772.9928, 33.442722}, -29.0f, &motel_ext };
	static auto motel_ent_10 = Marker{ {550.0185, -1770.6432, 33.442627}, -115.0f, &motel_ext };
	static auto motel_ent_11 = Marker{ {552.5525, -1765.29, 33.442627}, -115.0f, &motel_ext };
	static auto motel_ent_12 = Marker{ {555.4652, -1758.7952, 33.442627}, -115.0f, &motel_ext };
	static auto motel_ent_14 = Marker{ {559.20654, -1750.7898, 33.442627}, -115.0f, &motel_ext };
	static auto motel_ent_15 = Marker{ {561.8263, -1747.1501, 33.442627}, 148.0f, &motel_ext };

	static Marker* markers[] = {
		&ext_carclub,
		&ent_carclub,
		&ext_carclub_track,
		&ent_carclub_track,
		&ext_server_room,
		&ent_server_room,
		&ext_ranch,
		&ent_ranch,
		&ext_bahamamamas,
		&ent_bahamamamas,
		&ext_therapy,
		&ent_therapy,
		&sol_ext,
		&sol_ent_1,
		&sol_ent_2,
		&ext_benny,
		&ent_benny,
		&ext_chopshop,
		&ent_chopshop,
		&ext_tequilala,
		&ent_tequilala,
		&ext_comedy,
		&ent_comedy,
		&ext_finbank,
		&ent_finbank,
		&ext_fib,
		&ent_fib,
		&ext_morgue,
		&ent_morgue,
		&spc_lost_mc_clubhouse,
		&ext_humane,
		&ent_humane,

		&ext_stilthouse_1,
		&ent_stilthouse_1,

		&bunker_ext,
		&bunker_ent_1,
		&bunker_ent_2,
		&bunker_ent_3,
		&bunker_ent_4,
		&bunker_ent_5,
		&bunker_ent_6,
		&bunker_ent_7,
		&bunker_ent_8,
		&bunker_ent_9,
		&bunker_ent_10,
		&bunker_ent_11,

		&vehware_ext,
		&vehware_ent_1,
		&vehware_ent_2,
		&vehware_ent_3,
		&vehware_ent_4,
		&vehware_ent_5,
		&vehware_ent_6,
		&vehware_ent_7,
		&vehware_ent_8,
		&vehware_ent_9,

		&motel_ext,
		&motel_ent_1,
		&motel_ent_2,
		&motel_ent_3,
		&motel_ent_4,
		&motel_ent_5,
		&motel_ent_6,
		&motel_ent_7,
		&motel_ent_8,
		&motel_ent_9,
		&motel_ent_10,
		&motel_ent_11,
		&motel_ent_12,
		&motel_ent_14,
		&motel_ent_15,
	};

	bool MarkerBliponly::hasBlip() const noexcept
	{
		return blip != 0;
	}

	void MarkerBliponly::setBlip(bool toggle)
	{
		if (toggle)
		{
			if (!hasBlip())
			{
				blip = HUD::ADD_BLIP_FOR_COORD(pos.x, pos.y, pos.z);
				HUD::SET_BLIP_SPRITE(blip, 130);
				HUD::SET_BLIP_COLOUR(blip, 8);
				HUD::SET_BLIP_SCALE(blip, 0.4f);
				HUD::SET_BLIP_AS_SHORT_RANGE(blip, TRUE);
				HUD::SHOW_HEIGHT_ON_BLIP(blip, TRUE);
				HUD::SET_BLIP_NAME_FROM_TEXT_FILE(blip, "MC_GR_PROP_14");
			}
		}
		else
		{
			if (hasBlip())
			{
				BlipUtil::remove(blip);
				blip = 0;
			}
		}
	}

	void Marker::onEnter() const
	{
		if (exit != nullptr)
		{
			exit->onEnter();
		}
	}

	void Marker::onLeave() const
	{
		if (exit != nullptr)
		{
			exit->onLeave();
		}
	}

	void Marker::onTick() const
	{
		if (exit != nullptr)
		{
			exit->onTick();
		}
	}

	bool Marker::isEntrance() const noexcept
	{
		return exit != nullptr;
	}

	const Label& Marker::getName() const noexcept
	{
		if (exit != nullptr)
		{
			return exit->getName();
		}
		return name;
	}

	Marker* Marker::findEntrance() const noexcept
	{
		for (const auto& marker : markers)
		{
			if (marker->exit == this)
			{
				return marker;
			}
		}
		return nullptr;
	}

	v3 Marker::getRot() const noexcept
	{
		return { 0.0f, 0.0f, heading };
	}

	void Marker::draw() const
	{
		int r, g, b, a;
		HUD::GET_HUD_COLOUR(HUD_COLOUR_PINK, &r, &g, &b, &a);
		GRAPHICS::DRAW_MARKER(1, pos.x, pos.y, pos.z - 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.5f, r, g, b, 150, false, false, 2, false, nullptr, nullptr, false);

#if DOORS_DEV_MODE
		g_renderer.draw3dTextThisTickH(pos, pos, 1.0f, g_renderer.arColour);
		DrawUtil3d::draw_line(pos, pos + getRot().toDirNoZ());
#endif
	}

	void Marker::teleportAndWait() const
	{
		// Teleport there
		g_player_ent.setPos(pos);

		// Allow interior to load and force it in camera view
		auto viewmode = CAM::GET_FOLLOW_PED_CAM_VIEW_MODE();
		CAM::SET_FOLLOW_PED_CAM_VIEW_MODE(4);
		Script::current()->yield(500);
		CAM::SET_FOLLOW_PED_CAM_VIEW_MODE(viewmode);
		Script::current()->yield(500);

		// Finish transition
		CAM::DO_SCREEN_FADE_IN(500);
		postTpMove();
		Script::current()->yield(1500);
	}

	void Marker::postTpMove() const
	{
		v3 pos = this->pos;
		v3 rot = getRot();
		g_player_ent.setRot(rot);
		auto unit_vec = rot.toDir();
		if (g_player_veh.isValid())
		{
			pos += (unit_vec * 4.0f);
			TASK::TASK_VEHICLE_DRIVE_TO_COORD(g_player_ped, g_player_veh, pos.x, pos.y, pos.z, 3.0f, 1500, g_player_veh.getModel(), 0, 1.0f, 0.0f);
		}
		else
		{
			pos += (unit_vec * 2.0f);
			TASK::TASK_GO_STRAIGHT_TO_COORD(g_player_ped, pos.x, pos.y, pos.z, 1.0f, 1500, heading, 0.5f);
		}
	}

	CommandWorldDoors::CommandWorldDoors(CommandList* parent)
		: CommandToggle(parent, LOC("DOORS"), { CMDNAME("doors"), CMDNAME("enhancedopenworld") }, LOC("DOORS_H"))
	{
		instance = this;
	}

	bool CommandWorldDoors::isMarkerActive(const Marker* marker) const
	{
		if (marker->isEntrance())
		{
			if (isInInterior() && getCurrentInterior() == marker)
			{
				return false;
			}
		}
		else
		{
			if (!isInInterior() || getCurrentInterior()->exit != marker)
			{
				return false;
			}
		}
		if (online && (marker->flags & Marker::SP_ONLY))
		{
			return false;
		}
		return true;
	}

	bool CommandWorldDoors::isInInterior() const noexcept
	{
		return !interiors.empty();
	}

	Marker* CommandWorldDoors::getCurrentInterior() const noexcept
	{
		return interiors.top();
	}

	void CommandWorldDoors::forgetCurrentInterior() noexcept
	{
		interiors.pop();
	}

	void CommandWorldDoors::setBliponly(bool toggle) const
	{
		bliponly_jewel.setBlip(toggle);
		bliponly_max_renda_1.setBlip(toggle);
		bliponly_max_renda_2.setBlip(toggle);
		bliponly_eps.setBlip(toggle);
		bliponly_janitors.setBlip(toggle);
		bliponly_psb.setBlip(toggle);
		bliponly_torture.setBlip(toggle);
		bliponly_fameorshame.setBlip(toggle);
		bliponly_hospital.setBlip(toggle);
		bliponly_vagos.setBlip(toggle);
		bliponly_noose_storage.setBlip(toggle);
		bliponly_omega.setBlip(toggle);
		bliponly_lifeinvader.setBlip(toggle);

		bliponly_garage_1.setBlip(toggle);
		bliponly_garage_2.setBlip(toggle);
		bliponly_garage_3.setBlip(toggle);

		if (!toggle || !online)
		{
			bliponly_fib_lobby.setBlip(toggle);
			bliponly_lesters.setBlip(toggle);
			bliponly_fleeca_1.setBlip(toggle);
			bliponly_fleeca_2.setBlip(toggle);
			bliponly_fleeca_3.setBlip(toggle);
			bliponly_udg_1.setBlip(toggle);
			bliponly_udg_2.setBlip(toggle);

			bliponly_garage_sp_1.setBlip(toggle);
			bliponly_garage_sp_2.setBlip(toggle);
		}
	}

	void CommandWorldDoors::enableEntranceBlips() const
	{
		if (blips->m_on)
		{
			setBliponly(true);
			for (const auto& marker : markers)
			{
				marker->setBlip(
					marker->isEntrance()
					&& !(marker->flags & Marker::NO_BLIP)
					&& isMarkerActive(marker)
				);
			}
		}
	}

	void CommandWorldDoors::disableAllBlips() const
	{
		setBliponly(false);
		for (const auto& marker : markers)
		{
			marker->setBlip(false);
		}
	}

	void CommandWorldDoors::updateBlips() const
	{
		disableAllBlips();
		if (isInInterior())
		{
			if (blips->m_on)
			{
				getCurrentInterior()->exit->setBlip(true);
				if (getCurrentInterior() == &ent_carclub)
				{
					ent_carclub_track.setBlip(true);
				}
			}
		}
		else
		{
			enableEntranceBlips();
		}
	}

	static void pre_tp()
	{
		CAM::DO_SCREEN_FADE_OUT(500);
		while (!CAM::IS_SCREEN_FADED_OUT())
		{
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			Script::current()->yield();
		}
	}

	struct RequestIplRaii
	{
	private:
		const char* const name;

	public:
		explicit RequestIplRaii(const char* name)
			: name(STREAMING::IS_IPL_ACTIVE(name) ? nullptr : name)
		{
			if (this->name != nullptr)
			{
				STREAMING::REQUEST_IPL(this->name);
			}
		}

		~RequestIplRaii()
		{
			if (this->name != nullptr)
			{
				STREAMING::REMOVE_IPL(this->name);
			}
		}
	};

	struct RemoveIplRaii
	{
	private:
		const char* const name;

	public:
		explicit RemoveIplRaii(const char* name)
			: name(STREAMING::IS_IPL_ACTIVE(name) ? name : nullptr)
		{
			if (this->name != nullptr)
			{
				STREAMING::REMOVE_IPL(this->name);
			}
		}

		~RemoveIplRaii()
		{
			if (this->name != nullptr)
			{
				STREAMING::REQUEST_IPL(this->name);
			}
		}
	};

#define RAII_REQUEST_IPL(name) RequestIplRaii name(#name);
#define RAII_REMOVE_IPL(name) RemoveIplRaii name(#name);

	void CommandWorldDoors::onEnable(Click& click)
	{
		FiberPool::queueJob([this]
		{
			g_tb_deceive_interior_checks.enable();

			online = is_session_started();

			bool transition_wait = true;

			while (instance == this && m_on)
			{
				if (is_session_transition_active(true))
				{
					do
					{
						Script::current()->yield();
					} while (is_session_transition_active(true));

					if (transition_wait)
					{
						Script::current()->yield(5000);
						continue;
					}

					transition_wait = true;
				}

				g_ipl_hospital.enable();
				g_ipl_lost_mc_clubhouse.enable();
				g_ipl_fameorshame.enable();
				g_ipl_jewel.enable();
				g_ipl_max_renda.enable();
				g_ipl_fib_lobby.enable();
				g_ipl_lifeinvader.enable();

				// Union Depository
				RAII_REMOVE_IPL(DT1_03_Shutter);
				RAII_REMOVE_IPL(DT1_03_Gr_Closed);
				g_ipl_union_dep.enable();
				CommandListInteriors::udg->enable();

				// Epsilon Storage Room
				CommandListInteriors::eps->enable();
				force_door_open(1538555582, ATSTRINGHASH("v_ilev_epsstoredoor"), 241.3574f, 361.0488f, 105.8963f);

				// Janitor's
				force_door_open(-252283844, ATSTRINGHASH("v_ilev_janitor_frontdoor"), -107.5401f, -9.0258f, 70.6696f);

				// Torture
				force_door_open(963876966, ATSTRINGHASH("v_ilev_tort_door"), 134.4f, -2204.1f, 7.52f);

				// Lester's
				force_door_open(-565026078, ATSTRINGHASH("v_ilev_lester_doorfront"), 1274.0f, -1721.0f, 55.0f);

				// Vagos
				force_door_open(-1788473129, ATSTRINGHASH("prop_map_door_01"), -1104.66f, -1638.48f, 4.68f);

				// Pacific Standard Bank
				force_door_open(-366143778, ATSTRINGHASH("hei_v_ilev_bk_gate_pris"), 256.31f, 220.66f, 106.43f);
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(440819155, 0, FALSE, FALSE);

				// -1127.5768, 298.61896, 65.67667
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(904342475, 0, FALSE, FALSE);
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-795418380, 0, FALSE, FALSE);
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-1502457334, 0, FALSE, FALSE);
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-1994188940, 0, FALSE, FALSE);

				// Simeon's Car Dealership
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-1831288286, 0, FALSE, FALSE);
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-293141277, 0, FALSE, FALSE);
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(506750037, 0, FALSE, FALSE);
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(1496005418, 0, FALSE, FALSE);
				OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-1863079210, 0, FALSE, FALSE);

				RAII_REQUEST_IPL(gr_case0_bunkerclosed);
				RAII_REQUEST_IPL(gr_case1_bunkerclosed);
				RAII_REQUEST_IPL(gr_case2_bunkerclosed);
				RAII_REQUEST_IPL(gr_case3_bunkerclosed);
				RAII_REQUEST_IPL(gr_case4_bunkerclosed);
				RAII_REQUEST_IPL(gr_case5_bunkerclosed);
				RAII_REQUEST_IPL(gr_case6_bunkerclosed);
				RAII_REQUEST_IPL(gr_case7_bunkerclosed);
				RAII_REQUEST_IPL(gr_case9_bunkerclosed);
				RAII_REQUEST_IPL(gr_case10_bunkerclosed);
				RAII_REQUEST_IPL(gr_case11_bunkerclosed);

				enableEntranceBlips();

				while (true)
				{
#if DOORS_DEV_MODE
					drawDebugText(g_player_ped.getRot().z);
#endif

					if (isInInterior())
					{
						if (INTERIOR::GET_INTERIOR_FROM_ENTITY(g_player_ent) == 0)
						{
							pre_tp();
							getCurrentInterior()->teleportAndWait();
							getCurrentInterior()->onLeave();
							forgetCurrentInterior();
							enableEntranceBlips();
						}
					}
					else
					{
						// NOOSE Storage
						force_door_open(404057594, ATSTRINGHASH("prop_ch3_04_door_01l"), 2514.32f, -317.34f, 93.32f);
						force_door_open(-1417472813, ATSTRINGHASH("prop_ch3_04_door_01r"), 2512.42f, -319.26f, 93.32f);

						// Omega's Garage
						force_door_open(-1376084479, ATSTRINGHASH("prop_ch3_01_trlrdoor_l"), 2333.23f, 2574.97f, 47.03f);
						force_door_open(457472151, ATSTRINGHASH("prop_ch3_01_trlrdoor_r"), 2329.65f, 2576.64f, 47.03f);

						// Zancudo Bunker Elevator
						force_door_open(-621770121, ATSTRINGHASH("v_ilev_bl_doorel_l"), -2053.16f, 3239.49f, 30.5f);
						force_door_open(1018580481, ATSTRINGHASH("v_ilev_bl_doorel_r"), -2054.39f, 3237.23f, 30.5f);

						// Epsilon Gate
						OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(412198396, 0, FALSE, FALSE);
						OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-1053755588, 0, FALSE, FALSE);

						// Fleeca at Alta
						OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-93934272, 0, FALSE, FALSE);
						OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(667682830, 0, FALSE, FALSE);

						// Fleeca at Banham Canyon
						OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(1876735830, 0, FALSE, FALSE);
						OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-2112857171, 0, FALSE, FALSE);

						// Fleeca at Burton
						OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-2116116146, 0, FALSE, FALSE);
						OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(-74083138, 0, FALSE, FALSE);
					}

					const auto player_pos = g_player_ent.getPos();
					Marker* closest_marker;
					float closest_marker_dist = FLT_MAX;
					for (const auto& marker : markers)
					{
						if (isMarkerActive(marker))
						{
							auto dist = player_pos.distance(marker->pos);
							if (dist < 100.0f)
							{
								if ((marker->flags & Marker::PROXIMITY_TRIGGERS_MP_WORLD_STATE) && !WorldState::getOnline() && dist < 10.0f)
								{
									WorldState::setOnline(TRUE);
									online = !online;
									transition_wait = false;
									break;
								}
								if (!(marker->flags & Marker::NO_MARKER))
								{
									marker->draw();
								}
							}
							if (dist < closest_marker_dist)
							{
								closest_marker = marker;
								closest_marker_dist = dist;
							}
						}
					}

					if (online != is_session_started())
					{
						online = !online;
						break;
					}

					std::string hint;
					bool showing_hint = false;
					if (
						!(closest_marker->flags & Marker::NO_MARKER)
						&& !(closest_marker->flags & Marker::PROXIMITY_IS_CONTEXT)
						)
					{
						if (closest_marker->isEntrance())
						{
							hint = LANG_FMT("DOORS_ENT", fmt::arg("input", "~INPUT_CONTEXT~"), fmt::arg("interior", closest_marker->getName().getLocalisedUtf8()));
						}
						else
						{
							hint = LANG_FMT("DOORS_EXT", "~INPUT_CONTEXT~");
						}

						Util::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(hint);
						showing_hint = HUD::END_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(0);
					}

					if (!m_on)
					{
						if (showing_hint)
						{
							HUD::CLEAR_HELP(TRUE);
						}
						break;
					}

					if (closest_marker_dist >= (
						(
							!(closest_marker->flags & Marker::PROXIMITY_IS_CONTEXT)
							&& g_player_veh.isValid()
						)
						? 3.0f
						: 1.0f
						)
					)
					{
						if (showing_hint)
						{
							HUD::CLEAR_HELP(TRUE);
						}
					}
					else
					{
						if (
							(
								(closest_marker->flags & Marker::PROXIMITY_IS_CONTEXT)
								|| Input::isContextJustPressed()
							)
							&& !(closest_marker->flags & Marker::NO_MARKER)
							)
						{
							if (showing_hint)
							{
								HUD::CLEAR_HELP(TRUE);
							}
							pre_tp();
							Marker* dest;
							if (closest_marker->isEntrance())
							{
								closest_marker->onEnter();
								interiors.push(closest_marker);
								dest = closest_marker->exit;

								FiberPool::queueJob([this]
								{
									while (isInInterior())
									{
										getCurrentInterior()->onTick();
										Script::current()->yield();
									}
								});

								updateBlips();
							}
							else
							{
								if (isInInterior())
								{
									dest = getCurrentInterior();
									dest->onLeave();
									forgetCurrentInterior();
								}
								else
								{
									dest = closest_marker->findEntrance();
								}
								enableEntranceBlips();
							}
							dest->teleportAndWait();
						}
						else
						{
							if (!showing_hint)
							{
								Util::show_corner_help_impl(hint, true);
							}
						}
					}

					Script::current()->yield();
				}

				while (isInInterior())
				{
					g_player_ent.setPos(getCurrentInterior()->pos);
					getCurrentInterior()->onLeave();
					getCurrentInterior()->postTpMove();
					forgetCurrentInterior();
				}

				disableAllBlips();

				g_ipl_hospital.disable();
				g_ipl_lost_mc_clubhouse.disable();
				g_ipl_fameorshame.disable();
				g_ipl_jewel.disable();
				g_ipl_max_renda.disable();
				g_ipl_union_dep.disable();
				g_ipl_fib_lobby.disable();
				g_ipl_lifeinvader.disable();
			}

			g_tb_deceive_interior_checks.disable();
		});
	}
}
