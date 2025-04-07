#include "CommandListInfoOverlay.hpp"

#include <ctime>
#include <map>

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "AbstractModel.hpp"
#include "atArray.hpp"
#include "atFixedArray.hpp"
#include "CNetworkPlayerMgr.hpp"
#include "CNetworkSession.hpp"
#include "CommandCountryBreakdown.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaToggleScriptTick.hpp"
#include "CommandPlayer.hpp"
#include "CommandSessionCode.hpp"
#include "CommandWhosSpectatingWho.hpp"
#include "ComponentGpu.hpp"
#include "drawDebugText.hpp"
#include "FiberPool.hpp"
#include "fwPool.hpp"
#include "get_ground_z.hpp"
#include "gta_replayinterface.hpp"
#include "gta_vehicle.hpp"
#include "is_session.hpp"
#include "lang.hpp"
#include "main.hpp"
#include "Metrics.hpp"
#include "natives.hpp"
#include "NetInterface.hpp"
#include "pointers.hpp"
#include "ScriptMgr.hpp"
#include "script_thread.hpp"
#include "regular_event.hpp"
#include "Util.hpp"

namespace Stand
{
	static bool show_in_game_time = false;
	static bool show_real_world_time = false;
	static bool show_speedometer = false;
	static bool show_tps = false;
	static bool show_dps = false;
	static bool show_gpu_overhead = false;

	static bool show_gear = false;
	static bool show_rpm = false;

	static bool show_tag[FLAG_COUNT] = { false };
	static bool show_host_queue_sort_algo = false;
	static bool show_session_code = false;
	static bool show_ne_pool = false;

	static bool show_altitude = false;
	static bool show_ground_distance = false;
	static bool show_position = false;
	static bool show_rotation_ent = false;
	static bool show_rotation_cam = false;
	static bool show_peds = false;
	static bool show_vehs = false;
	static bool show_objs = false;
	static bool show_pups = false;
	static bool show_proj = false;

	static bool show_programs = false;
	static bool show_threads = false;
	static bool show_gta_natives = false;

	static bool show_stand_natives = false;
	static bool show_ent_handles = false;
	static bool show_ctrl_reqs = false;
	static bool show_fibers = false;
	static bool show_script_tick_handlers = false;
	static bool show_present_handlers = false;
	static bool show_net_interface_sockets = false;

	static void drawInfoRotation(const v3& rot)
	{
		drawInfoText(fmt::to_string(rot.x), LANG_GET("ROT_X"));
		drawInfoText(fmt::to_string(rot.y), LANG_GET("ROT_Y"));
		drawInfoText(fmt::to_string(rot.z), LANG_GET("ROT_Z"));
	}

	static void onTick()
	{
		if (show_in_game_time)
		{
			drawDebugText(fmt::format(L"{:0>2}:{:0>2}:{:0>2}", CLOCK::GET_CLOCK_HOURS(), CLOCK::GET_CLOCK_MINUTES(), CLOCK::GET_CLOCK_SECONDS()));
		}
		if (show_real_world_time)
		{
			std::time_t t = std::time(nullptr);
			std::tm* now = std::localtime(&t);
			drawDebugText(fmt::format(L"{:0>2}:{:0>2}:{:0>2}", now->tm_hour, now->tm_min, now->tm_sec));
		}
		if (show_speedometer)
		{
			auto pair = Util::mps_to_unit_of_choice_pair_w(g_player_ent.getSpeed());
			drawInfoNumber(pair.first, pair.second);
		}
		if (show_tps)
		{
			drawInfoNumber(Metrics::ticks_per_second, L"TPS");
		}
		if (show_dps)
		{
			drawInfoNumber(Metrics::draws_per_second, L"DPS");
		}
		if (show_gpu_overhead)
		{
			drawInfoText(fmt::to_string(g_comp_gpu.overhead_ms), LANG_GET("GPUOVHD"));
		}

		if (show_gear)
		{
			if (auto ptr = g_player_veh.getCVehicle(); ptr && !g_player_veh.getModel().isAirborne())
			{
				if (ptr->m_Transmission.m_nPrevGear == 0)
				{
					drawInfoNumber(LANG_GET("GEAR_R"), LANG_GET("GEAR"));
				}
				else
				{
					drawInfoNumber(ptr->m_Transmission.m_nPrevGear, LANG_GET("GEAR"));
				}
			}
			else
			{
				drawInfoNumber(LANG_GET("NA"), LANG_GET("GEAR"));
			}
		}
		if (show_rpm)
		{
			if (auto ptr = g_player_veh.getCVehicle(); ptr && !g_player_veh.getModel().isAirborne())
			{
				drawInfoNumber(int32_t(ptr->m_Transmission.rpm * 10000.0f), LANG_GET("RPM"));
			}
			else
			{
				drawInfoNumber(LANG_GET("NA"), LANG_GET("RPM"));
			}
		}

		if (show_tag[FLAG_SELF])
		{
			drawInfoNumber(AbstractPlayer::getPlayerCount(), ATSTRINGHASH("PLY"), ATSTRINGHASH("PLYLST"));
		}
		for (playerflag_t f = FLAG_FRIEND; f != FLAG_COUNT; ++f)
		{
			if (show_tag[f])
			{
				switch (f)
				{
				case FLAG_HOST:
					{
						auto h = AbstractPlayer::getHost();
						drawInfoText(h.isValid() ? h.getName() : LANG_GET("NA"), LANG_GET("FLAG_HOST"));
					}
					break;

				case FLAG_NEXT_HOST:
					{
						auto next_host = AbstractPlayer::getNextHost();
						drawInfoText(next_host.isValid() ? next_host.getName() : LANG_GET("NA"), LANG_GET("NXTHST"));
					}
					break;

				case FLAG_SCRIPT_HOST:
					{
						auto h = AbstractPlayer::getScriptHost();
						drawInfoText(h.isValid() ? h.getName() : LANG_GET("NA"), LANG_GET("FLAG_SCRIPT_HOST"));
					}
					break;

				default:
					{
						player_t count = 0;
						for (auto p : AbstractPlayer::listAll())
						{
							if (auto cmd = p.getCommand())
							{
								if ((cmd->flags >> f) & 1)
								{
									++count;
								}
							}
						}
						drawInfoNumber(
							count,
							PlayerTag_getLabel(f).getLocalisationHash(),
							PlayerTag_getPluralLabel(f).getLocalisationHash()
						);
					}
				}
			}
		}
		if (show_host_queue_sort_algo)
		{
			if (is_session_or_transition_active(false))
			{
				switch ((*pointers::network_session)->host_queue_sort_algo)
				{
				case 0: drawInfoText(LANG_GET("HQSA_0"), LANG_GET("HQSA")); break;
				case 1: drawInfoText(LANG_GET("HQSA_1"), LANG_GET("HQSA")); break;
				case 2: drawInfoText(LANG_GET("HQSA_2"), LANG_GET("HQSA")); break;
				case 3: drawInfoText(LANG_GET("HQSA_3"), LANG_GET("HQSA")); break;
				}
			}
		}
		if (show_session_code)
		{
			drawInfoText(CommandSessionCode::getSessionCodeForUser(), LANG_GET("SESSCODE"));
		}
		if (show_ne_pool)
		{
			if (*pointers::rage_netGameEvent_ms_pPool)
			{
				drawInfoText(fmt::format(
					fmt::runtime(soup::ObfusString("{}/{}").str()),
					(*pointers::rage_netGameEvent_ms_pPool)->GetNoOfUsedSpaces(),
					(*pointers::rage_netGameEvent_ms_pPool)->GetSize()
				), LANG_GET("NEPOOL"));
			}
		}

		if (show_altitude)
		{
			auto pos = AbstractEntity::getEffectiveUserPos();
			drawInfoNumber(pos.z, LANG_GET("ALTDE"));
		}
		if (show_ground_distance)
		{
			auto pos = AbstractEntity::getEffectiveUserPos();
			drawInfoNumber(pos.z - get_ground_z(pos.x, pos.y, pos.z), LANG_GET("GRNDIST"));
		}
		if (show_position)
		{
			auto pos = AbstractEntity::getEffectiveUserPos();
			drawInfoText(fmt::to_string(pos.x), "X");
			drawInfoText(fmt::to_string(pos.y), "Y");
			drawInfoText(fmt::to_string(pos.z), "Z");
		}
		if (show_rotation_ent)
		{
			drawInfoRotation(g_player_ent.getRot());
		}
		if (show_rotation_cam)
		{
			drawInfoRotation(CAM::GET_FINAL_RENDERED_CAM_ROT(2));
		}
		if (show_peds)
		{
			uint64_t count = 0;
			if (CReplayInterfaceGame* const replay = *pointers::replay_interface)
			{
				if (auto* const ped_interface = replay->ped_interface)
				{
					for (const auto& ent : ped_interface->list->entities)
					{
						if (ent.ptr != nullptr)
						{
							count++;
						}
					}
				}
			}
			drawInfoNumber(count, LANG_GET("PEDS"));
		}
		if (show_vehs)
		{
			uint64_t count = 0;
			if (CReplayInterfaceGame* const replay = *pointers::replay_interface)
			{
				if (auto* const veh_interface = replay->veh_interface)
				{
					for (const auto& ent : veh_interface->list->entities)
					{
						if (ent.ptr != nullptr)
						{
							count++;
						}
					}
				}
			}
			drawInfoNumber(count, LANG_GET("VEHS"));
		}
		if (show_objs)
		{
			uint64_t count = 0;
			if (CReplayInterfaceGame* const replay = *pointers::replay_interface)
			{
				if (auto* const object_interface = replay->object_interface)
				{
					for (const auto& ent : object_interface->list->entities)
					{
						if (ent.ptr != nullptr)
						{
							count++;
						}
					}
				}
			}
			drawInfoNumber(count, LANG_GET("OBJS"));
		}
		if (show_pups)
		{
			uint64_t count = 0;
			if (CReplayInterfaceGame* const replay = *pointers::replay_interface)
			{
				if (auto* const pickup_interface = replay->pickup_interface)
				{
					for (const auto& ent : pickup_interface->list->entities)
					{
						if (ent.ptr != nullptr)
						{
							count++;
						}
					}
				}
			}
			drawInfoNumber(count, LANG_GET("PUPS"));
		}
		if (show_proj)
		{
			drawInfoText(fmt::format(fmt::runtime(soup::ObfusString("{}/50").str()), pointers::projectile_pool->m_Count), LANG_GET("PROJ"));
		}

		if (show_programs)
		{
			if (*pointers::CLoadingScreens_ms_Context == 0)
			{
				uint64_t count = 0;
				for (uint64_t i = 0; i < pointers::script_programs_size; i++)
				{
					if (pointers::script_programs[i])
					{
						count++;
					}
				}
				drawInfoNumber(count, LANG_GET("SCPROG"));
			}
		}
		if (show_threads)
		{
			if (*pointers::CLoadingScreens_ms_Context == 0)
			{
				uint64_t count = 0;
				for (auto thread : *pointers::script_threads)
				{
					if (thread && thread->m_context.m_thread_id)
					{
						count++;
					}
				}
				drawInfoNumber(count, LANG_GET("SCTHDS"));
			}
		}
		if (show_gta_natives)
		{
			drawInfoNumber(Metrics::natives_game_invoked_last_tick, LANG_GET("NGILT"));
		}

		if (show_stand_natives)
		{
			drawInfoNumber(Metrics::natives_stand_invoked_last_tick, LANG_GET("NSILT"));
		}
		if (show_ent_handles)
		{
			drawInfoNumber(Metrics::entity_handles_last_tick, LANG_GET("EHLT"));
		}
		if (show_ctrl_reqs)
		{
			drawInfoNumber(AbstractEntity::control_requests.size(), LANG_GET("CTRLREQS"));
		}
		if (show_fibers)
		{
			drawInfoNumber(g_script_mgr.getNumScripts(g_hmodule), LANG_GET("FIBS"));
		}
		if (show_script_tick_handlers)
		{
			drawInfoNumber(reScriptTickEvent::getNumHandlers() + rePostTickEvent::getNumHandlers(), LANG_GET("STH"));
		}
		if (show_present_handlers)
		{
			drawInfoNumber(rePresentEvent::getNumHandlers() + g_renderer.asi_present_callbacks.size(), LANG_GET("PRSNTHDLS"));
		}
		if (show_net_interface_sockets)
		{
			drawInfoNumber(g_net_interface.getNumSockets(), LANG_GET("KACONS"));
		}
	}

	static uint8_t func_ref = 0;

	class CommandToggleInfo : public CommandToggle
	{
	private:
		bool* const ptr;

	public:
		explicit CommandToggleInfo(CommandList* const parent, bool* const ptr, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), ptr(ptr)
		{
		}

		void autogenerateCommandNames()
		{
			command_names = combineCommandNames(CMDNAME("info"), getCommandNamesFromMenuName(menu_name));
		}

		void onEnable(Click& click) final
		{
			*ptr = true;
			if (++func_ref == 1)
			{
				ensureScriptThread(click, []
				{
					reScriptTickEvent::registerHandler(&onTick);
				});
			}
		}

		void onDisable(Click& click) final
		{
			*ptr = false;
			if (--func_ref == 0)
			{
				ensureScriptThread(click, []
				{
					reScriptTickEvent::unregisterHandler(&onTick);
				});
			}
		}
	};

	CommandListInfoOverlay::CommandListInfoOverlay(CommandList* const parent)
		: CommandList(parent, LOC("HUDINFO"), { CMDNAME("hudinfo"), CMDNAME("infooverlay"), CMDNAME("infoverlay") })
	{
		this->createChild<CommandToggleInfo>(&show_in_game_time, LOC("IGTIME"), CMDNAMES("infoclock"));
		this->createChild<CommandToggleInfo>(&show_real_world_time, LOC("RWTIME"), CMDNAMES("infotime"));
		this->createChild<CommandToggleInfo>(&show_speedometer, LOC("PLATESPEED"), { CMDNAME("infospeed") });
		this->createChild<CommandToggleInfo>(&show_tps, LOC("TS"), CMDNAMES("infotps", "infofps"), LOC("TS_H"));
		this->createChild<CommandToggleInfo>(&show_dps, LOC("DPS"), CMDNAMES("infodps"));
		this->createChild<CommandToggleInfo>(&show_gpu_overhead, LOC("GPUOVHD"), CMDNAMES("infogpuoverhead"), LOC("GPUOVHD_H"));

		/* Vehicle */ this->createChild<CommandDivider>(LOC("VEH"));
		this->createChild<CommandToggleInfo>(&show_gear, LOC("GEAR"), CMDNAMES("infogear"));
		this->createChild<CommandToggleInfo>(&show_rpm, LOC("RPM"), CMDNAMES("inforpm"));

		/* Online */ this->createChild<CommandDivider>(LOC("Online"));
		this->createChild<CommandToggleInfo>(&show_tag[FLAG_SELF], LOC("PLYLST"), CMDNAMES("infoplayers"));
		{
			// Player Tags
			auto tags = this->createChild<CommandList>(LOC("FLAGS_A"));
#define CREATE_TAG_TOGGLE(tag, label) \
if constexpr (tag != FLAG_SELF) \
{ \
	tags->createChild<CommandToggleInfo>(&show_tag[tag], LOC(label))->autogenerateCommandNames(); \
}
			FOR_ALL_FLAGS(CREATE_TAG_TOGGLE)
		}
		this->createChild<CommandToggleInfo>(&show_host_queue_sort_algo, LOC("HQSA"), { CMDNAME("infohostqueue") });
		this->createChild<CommandWhosSpectatingWho>();
		this->createChild<CommandToggleInfo>(&show_session_code, LOC("SESSCODE"), { CMDNAME("infosessioncode") });
		this->createChild<CommandCountryBreakdown>();
		this->createChild<CommandToggleInfo>(&show_ne_pool, LOC("NEPOOL"), { CMDNAME("infonetworkevents"), CMDNAME("infonepool") });

		/* World */ this->createChild<CommandDivider>(LOC("WRLD"));
		this->createChild<CommandToggleInfo>(&show_altitude, LOC("ALTDE"), CMDNAMES("infoaltitude"));
		this->createChild<CommandToggleInfo>(&show_ground_distance, LOC("GRNDIST"), CMDNAMES("infogrounddistance", "infogroundistance"));
		this->createChild<CommandToggleInfo>(&show_position, LOC("POS"), CMDNAMES("infoposition"));
		this->createChild<CommandToggleInfo>(&show_rotation_ent, LOC("ROT_ENT"), CMDNAMES("inforotationentity"));
		this->createChild<CommandToggleInfo>(&show_rotation_cam, LOC("ROT_CAM"), CMDNAMES("inforotationcamera"));
		this->createChild<CommandToggleInfo>(&show_peds, LOC("PEDS"), CMDNAMES("infopeds"));
		this->createChild<CommandToggleInfo>(&show_vehs, LOC("VEHS"), CMDNAMES("infovehicles"));
		this->createChild<CommandToggleInfo>(&show_objs, LOC("OBJS"), CMDNAMES("infoobjects", "infobjects", "infoobjs", "infobjs"));
		this->createChild<CommandToggleInfo>(&show_pups, LOC("PUPS"), CMDNAMES("infopickups"));
		this->createChild<CommandToggleInfo>(&show_proj, LOC("PROJ"), CMDNAMES_OBF("infoprojectiles"));

		/* Game */ this->createChild<CommandDivider>(LOC("GME"));
		this->createChild<CommandToggleInfo>(&show_programs, LOC("SCPROG"), CMDNAMES("infoscriptprograms", "infoprograms"));
		this->createChild<CommandToggleInfo>(&show_threads, LOC("SCTHDS"), CMDNAMES("infoscriptthreads", "infoscripthreads", "infothreads"));
		this->createChild<CommandToggleInfo>(&show_gta_natives, LOC("NGILT"), CMDNAMES("infogtanatives", "infonativesgta"));

		/* Stand */ this->createChild<CommandDivider>(LIT("Stand"));
		this->createChild<CommandToggleInfo>(&show_stand_natives, LOC("NSILT"), CMDNAMES("infostandnatives", "infonativesstand", "infonativestand"));
		this->createChild<CommandToggleInfo>(&show_ent_handles, LOC("EHLT"), CMDNAMES("infoentityhandles"));
		this->createChild<CommandToggleInfo>(&show_ctrl_reqs, LOC("CTRLREQS"), CMDNAMES("infocontrolrequests"));
		this->createChild<CommandToggleInfo>(&show_fibers, LOC("FIBS"), CMDNAMES("infofibers"));
		this->createChild<CommandToggleInfo>(&show_script_tick_handlers, LOC("STH"), CMDNAMES("infoscripttick", "infoscriptick"));
		this->createChild<CommandToggleInfo>(&show_present_handlers, LOC("PRSNTHDLS"), CMDNAMES("infodraw"));
		this->createChild<CommandToggleInfo>(&show_net_interface_sockets, LOC("KACONS"), CMDNAMES("infokeepalive"));
	}
}
