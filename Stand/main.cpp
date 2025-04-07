#include "main.hpp"

#include <discord_rpc.h>

#include <soup/log.hpp>
#include <soup/ObfusString.hpp>
#include <soup/os.hpp>
#include <soup/sha1.hpp>
#include <soup/string.hpp>

#include "AntiDbg.hpp"
#include "Auth.hpp"
#include "CNetworkAssetVerifier.hpp"
#include "CodeIntegrity.hpp"
#include "ColoadMgr.hpp"
#include "ColonsAndTabs.hpp"
#include "CommandHistoricPlayer.hpp"
#include "Components.hpp"
#include "conf.hpp"
#include "ConsoleLogger.hpp"
#include "CScaleformStore.hpp"
#include "get_root_list.hpp"
#include "Exceptional.hpp"
#include "get_appdata_path.hpp"
#include "get_current_time_millis.hpp"
#include "get_seconds_since_unix_epoch.hpp"
#include "CPlayerInfo.hpp"
#include "gta_ped.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "HttpRequestBuilder.hpp"
#include "is_session.hpp"
#include "joaatToString.hpp"
#include "JoinUtil.hpp"
#include "FileLogger.hpp"
#include "MapUtil.hpp"
#include "Matchmaking.hpp"
#include "MenuGrid.hpp"
#include "mystackwalker.hpp"
#include "natives.hpp"
#include "NativeTableHooksBuiltin.hpp"
#include "NetInterface.hpp"
#include "PatternBatch.hpp"
#include "PeHeader.hpp"
#include "PointerCache.hpp"
#include "pointers.hpp"
#include "PubSubClient.hpp"
#include "RageConnector.hpp"
#include "RelayCon.hpp"
#include "RemoteStats.hpp"
#include "Renderer.hpp"
#include "ResourceMgr.hpp"
#include "rlGamerInfo.hpp"
#include "Sanity.hpp"
#include "ScriptMgr.hpp"
#include "script_program.hpp"
#include "ScriptVmErrorHandling.hpp"
#include "ScriptVmOpCode.hpp"
#include "scrNativeRegistrationTable.hpp"
#include "SessionSpoofing.hpp"
#include "str2int.hpp"
#include "StringUtils.hpp"
#include "TimedCall.hpp"
#include "Tunables.hpp"
#include "Util.hpp"

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		return ::Stand::onDllAttach(hmod);
	}
	return TRUE;
}

// PinkEye helper
extern "C"
{
	__declspec(dllexport) LRESULT Deez(int code, WPARAM wParam, LPARAM lParam)
	{
		return CallNextHookEx(nullptr, code, wParam, lParam);
	}
}

namespace Stand
{
	extern "C"
	{
		// script_vm_helper.asm
		using on_invalid_op_t = void(*)(int64_t opcode);
		using on_error_t = uint32_t(*)();

		void script_vm_helper_init_1(void** const jump_table, const on_invalid_op_t on_invalid_op);
		void script_vm_helper_init_2(const on_error_t on_error, void* const vm_return);

		void script_vm_op_native_init(void** const og_native_op);

		// swapchain_hook.asm
		void swapchain_hook_init_present(void* detour, void* jumpout);
		void swapchain_hook_init_resize_buffers(void* og_call, void* detour, void* jumpout);

		// data_node_helper.asm
		using data_node_write_detour_t = void(__fastcall*)(rage::netSyncDataNode*, rage::netSyncTreeTargetObject*, rage::datBitBuffer&);

		void data_node_helper_init(data_node_write_detour_t detour, void* ret);

		// rage_input_helper.asm
		void set_value_from_keyboard_init(void* ret);
		void set_value_from_mkb_axis_init_ret_og(void* ret);
		void set_value_from_mkb_axis_init_ret_bail(void* ret);

		// aes_helper.asm
		void aes_initfile_init(void* call, void* cont);
		void aes_decrypt_init(void* call, void* cont);

		// overrun_helper.asm
		void net_event_error_init(void* cont);
	}

	static void showMessageBoxAndLog(const char* message)
	{
		g_logger.log(message);
		MessageBoxA(nullptr, message, soup::ObfusString(STAND_NAMEVERSION).c_str(), MB_OK | MB_ICONEXCLAMATION);
	}

#define STORE_HOOK(name) g_hooking.name##_hook.setTarget(p.as<void*>());

#define BATCH_ADD_MANDATORY_HOOK(name, pname, head_sig, body_sig) BATCH_ADD_FUNC_IMPL(name, pname, head_sig, body_sig, add, STORE_HOOK)
#define BATCH_ADD_OPTIONAL_HOOK(name, pname, head_sig, body_sig) BATCH_ADD_FUNC_IMPL(name, pname, head_sig, body_sig, add_optional, STORE_HOOK)

#define IS_TAKING_TOO_LONG(target) (counter < target && ++counter == target)

	static bool early_inject = false;
	static std::string memory_pool_str_buf{};
	static bool modified_memory_pool_size = false;
	static bool did_minimal_init = false;
	static bool patched_is_explosion_type_valid = false;

	enum /*CrossState*/ : uint16_t
	{
		CS_NONE = 0xC032, // xor al, al
#if ENABLE_PASSIVE_DLL
		CS_PASSIVE = 0x00B0, // mov al, 0
#endif
		CS_ACTIVE = 0x01B0, // mov al, 1
	};

	[[nodiscard]] static bool is_special_key_being_pressed() noexcept
	{
		return GetForegroundWindow() == pointers::hwnd
			&& (
				(GetAsyncKeyState(VK_F1) & 0x8000)
				|| (GetAsyncKeyState(VK_F9) & 0x8000)
			);
	}

	[[nodiscard]] static bool has_game_finished_loading()
	{
		/*static time_t emulation = 0;
		if (emulation == 0)
		{
			emulation = get_current_time_millis();
		}
		return (GET_MILLIS_SINCE(emulation) > 30000);*/

		return *pointers::CLoadingScreens_ms_Context == 0 && *pointers::ped_factory != nullptr && (*pointers::ped_factory)->m_local_ped != nullptr;
	}

	static void call_with_time_limit(void f(uintptr_t), uintptr_t ud = 0)
	{
		auto t = get_current_time_millis();
		f(ud);
		SOUP_IF_UNLIKELY (GET_MILLIS_SINCE(t) > 1000)
		{
			AntiDbg::apologiseInAdvance(1);
			AntiDbg::bToTheSToTheOToTheD();
		};
	}

	static void ensureAntiDump()
	{
		// Avoid process-dump picking up on DLL as codechunk
		for (size_t i = 0x1000; i < g_stand_dll_range.size; i += 0x1000)
		{
			DWORD OldProtect;
			VirtualProtect(g_hmodule + i, 0x1000, 0, &OldProtect);
		}
	}

	BOOL onDllAttach(HMODULE hmod) // OBFUS!
	{
		// Initialise
		DisableThreadLibraryCalls(hmod);
		g_logger.init(FileLogger::getMainFilePath());
		g_logger.log(soup::ObfusString(STAND_NAMEVERSION " reporting for duty!"));
		// Check debugger
		if (auto dbgid = AntiDbg::getRunningDebuggerId(true))
		{
			if (dbgid >= 100)
			{
				dbgid -= 100;
			}
			g_logger.log(fmt::format("Startup failed ({})", dbgid));
			mainUnload();
			return FALSE;
		}
		// Start exception handling
		g_hmodule = hmod;
		g_og_unhandled_exception_filter = Exceptional::setUnhandledExceptionHandler();
		Exceptional::createManagedThread(&Exceptional::thread_func);
		::exceptional_init(&Exceptional::handleCaughtException, &Exceptional::handleUncaughtException);
		// Initialise CodeIntegrity
		CodeIntegrity::add(&CommandHistoricPlayer::getStateImpl);
		CodeIntegrity::add(static_cast<std::string(*)(const void*, size_t)>(&soup::sha1::hash));
		// Detect OS version (note that this does not work to detect Windows 8 or later)
#pragma warning(push)
#pragma warning(disable: 4996) // GetVersion was declared deprecated
		DWORD dwVersion = GetVersion();
#pragma warning(pop)
		DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
		g_windows_7_or_older = (dwMajorVersion <= 6 && dwMinorVersion <= 1);
		if (g_windows_7_or_older)
		{
			g_logger.log(soup::ObfusString("Detected an ancient Windows version. I'll try my best to adapt by disabling incompatible code, but you're in for a seriously degraded experience."));
		}
		// Init main
		__try
		{
			mainInit();
		}
		__EXCEPTIONAL()
		{
		}
		// Init main thread
		auto th = Exceptional::createThread(&mainThread);
		if (th)
		{
			SetThreadPriority(th, THREAD_PRIORITY_ABOVE_NORMAL);
			CloseHandle(th);
		}
		return TRUE;
	}

	void mainInit()
	{
		// Detect early inject
		std::wstring sGrcWindow = StringUtils::utf8_to_utf16(soup::ObfusString("grcWindow").str());
		if (!FindWindowW(sGrcWindow.c_str(), nullptr)
			|| GetModuleHandleA(soup::ObfusString("socialclub.dll")) == nullptr
			)
		{
			early_inject = true;
		}

		// Remember who we are
		g_stand_dll_file_name = Util::get_module_file_name(g_hmodule);
		g_stand_dll_range = soup::Module(g_hmodule).range;

		// Detect if we're running under Wine
		g_wine = soup::os::isWine();

		// Remove DLL from PEB
		enable_module_hider_if_applicable();

		// Erase PE Header
#if ERASE_PE_HEADER
		call_with_time_limit([](uintptr_t)
		{
			DWORD OldProtect;
			VirtualProtect(g_hmodule, 0x1000, PAGE_READWRITE, &OldProtect);
			PeHeader::pe_header.initPatchZero((uint8_t*)g_hmodule, 4096);
		});
#endif

		ensureAntiDump();

		if (early_inject)
		{
			mainInitDeferables();

			{
				std::string startup_msg = soup::ObfusString("Stealy V-eely Automobiley is starting up.").str();
				if (MapUtil::hasKeyValue(g_gui.active_profile.data, soup::ObfusString("Game>Early Inject Enhancements>Set Game To High Priority").str(), soup::ObfusString("On").str()))
				{
					SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
					startup_msg.append(soup::ObfusString(" Set process to high priority.").str());
				}
				g_logger.log(std::move(startup_msg));
			}

			if (g_gui.active_profile.data.contains(soup::ObfusString("Game>Early Inject Enhancements>Memory Pool Size (MB)").str()))
			{
				PatternBatch batch;
				BATCH_ADD("A0", "83 C8 01 48 8D 0D ? ? ? ? 41 B1 01 45 33 C0", [](soup::Pointer p)
				{
					if (*p.add(23).rip().as<uint32_t*>() & 1)
					{
						memory_pool_str_buf = soup::ObfusString("Can't modify memory pool size as it is already initiated. Pattern scan took {}.").str();
					}
					else
					{
						int mb = str2int<int>(g_gui.active_profile.data.at(soup::ObfusString("Game>Early Inject Enhancements>Memory Pool Size (MB)").str())).value();
						if (mb == 2147)
						{
							*p.add(17).as<int32_t*>() = 0x7FFFFFFF;
						}
						else
						{
							*p.add(17).as<int32_t*>() = (mb * 1000000);
						}
						memory_pool_str_buf = soup::ObfusString("Modified memory pool size in {}.").str();
						modified_memory_pool_size = true;
					}
				});
				auto time = TimedCall::run([&]
				{
					batch.runSingleThread();
				});
				if (batch.error_message.empty())
				{
					g_logger.log(fmt::format(fmt::runtime(memory_pool_str_buf), time));
				}
				else
				{
					g_logger.log(soup::ObfusString("Memory Pool Size: Pattern scan failed."));
				}
				memory_pool_str_buf.clear();
			}
		}
	}

	void mainInitDeferables()
	{
		// Initial file structure
		{
			auto path = get_appdata_path();
			if (!std::filesystem::exists(path))
			{
				std::filesystem::create_directory(path);
			}
			path.append(StringUtils::utf8_to_utf16(soup::ObfusString("Profiles\\").str()));
			if (!std::filesystem::exists(path))
			{
				std::filesystem::create_directory(path);
			}
		}

		// Load state
		{
			g_gui.meta_state.load();
			const bool got_tutorial_flags_from_meta_state = g_gui.loadTutorialFlags(g_gui.meta_state.data);

			std::string state_name;
			auto e = g_gui.meta_state.data.find(soup::ObfusString("Load On Inject").str());
			if (e != g_gui.meta_state.data.end())
			{
				state_name = e->second;
			}
			else
			{
				e = g_gui.meta_state.data.find(soup::ObfusString("Active Profile").str()); // pre-116
				if (e != g_gui.meta_state.data.end())
				{
					state_name = e->second;
				}
				else
				{
					state_name = soup::ObfusString("Main").str();
				}
				g_gui.meta_state.data.emplace(soup::ObfusString("Load On Inject").str(), state_name);
				g_gui.meta_state.save();
			}
			if (!state_name.empty())
			{
				state_name.insert(0, soup::ObfusString("Profiles\\").str());
				g_gui.active_profile.setName(std::move(state_name));
				g_gui.loadStateToMemory(g_gui.active_profile);
				if (!got_tutorial_flags_from_meta_state)
				{
					if (g_gui.loadTutorialFlags(g_gui.active_profile.data))
					{
						g_gui.meta_state.save();
					}
				}
			}

			e = g_gui.meta_state.data.find(soup::ObfusString("Autosave").str());
			if (e != g_gui.meta_state.data.end())
			{
				g_gui.enableAutosave(StringUtils::utf8_to_utf16(e->second));
			}
		}
	}

	struct LogSink : public soup::logSink
	{
		void write(std::string&& message) final
		{
			g_logger.log_no_nl(std::move(message));
		}
	};

	void mainThread()
	{
		THREAD_NAME("Main");
		__try
		{
			if (!early_inject)
			{
				mainInitDeferables();
			}

			soup::logSetSink(soup::make_unique<LogSink>());

			g_hooking.init();

			bool load_aborted = false;

			if (early_inject)
			{
				std::wstring sGrcWindow = StringUtils::utf8_to_utf16(soup::ObfusString("grcWindow").str());
				while (!FindWindowW(sGrcWindow.c_str(), nullptr))
				{
					soup::os::sleep(10);
				}

				if (auto e = g_gui.active_profile.data.find(soup::ObfusString("Game>Early Inject Enhancements>Game Pools Size Multiplier").str()); e != g_gui.active_profile.data.end())
				{
					g_hooking.pools_size_multiplier = std::stof(e->second);
					if (g_hooking.pools_size_multiplier > 2.0f
						&& !modified_memory_pool_size
						)
					{
						g_hooking.pools_size_multiplier = 2.0f;
						g_logger.log(soup::ObfusString("Game Pools Size Multiplier: Capping to 2.0 as memory pool size was not modified.").str());
					}
					PatternBatch batch;
					BATCH_ADD("A1", "45 33 DB 44 8B D2 66 44 39 59 10 74 4B 44 0F B7 49 10", [](soup::Pointer p)
					{
						STORE_HOOK(rage_fwConfigManager_GetSizeOfPool);
					});
					auto scan_time = TimedCall::run([&]
					{
						batch.runSingleThread();
					});
					if (batch.error_message.empty())
					{
						auto hook_time = TimedCall::run([]
						{
							g_hooking.rage_fwConfigManager_GetSizeOfPool_hook.createHook();
							g_hooking.rage_fwConfigManager_GetSizeOfPool_hook.enableHook();
						});
						g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Game Pools Size Multiplier: Scanned pattern in {} and hooked in {}").str()), scan_time, hook_time));
					}
					else
					{
						g_logger.log(soup::ObfusString("Game Pools Size Multiplier: Pattern scan failed."));
					}
				}

				std::string message{};
				if (MapUtil::hasKeyValue(g_gui.active_profile.data, soup::ObfusString("Game>Early Inject Enhancements>Skip Intro & License").str(), soup::ObfusString("Off").str()))
				{
					message = soup::ObfusString("Not skipping intro and license: Disabled in state").str();
				}
				else
				{
					PatternBatch batch;
					BATCH_ADD("A2", "70 6C 61 74 66 6F 72 6D 3A 2F 6D 6F 76 69 65 73 2F 72 6F 63", [](soup::Pointer p)
					{
						pointers::game_logos = p.as<void*>();
					});
					BATCH_ADD("A3", "72 1F E8 ? ? ? ? 8B 0D", [](soup::Pointer p)
					{
						pointers::game_license = p.as<void*>();
					});
					batch.run();
					if (batch.error_message.empty())
					{
						uint8_t value = 0xC3;
						memcpy(pointers::game_logos, &value, sizeof(value));
						memset(pointers::game_license, 0x90, 2);
						message = soup::ObfusString("Skipped intro and license").str();
					}
					else
					{
						ColoadMgr::coloading_with_any_menu = true;
						message = soup::ObfusString("Not skipping intro and license: ").str();
						message.append(batch.error_message);
					}
				}
				message.append(soup::ObfusString(". Waiting for socialclub.").str());
				g_logger.log(std::move(message));

				while (GetModuleHandleA(soup::ObfusString("socialclub.dll")) == nullptr)
				{
					if (is_special_key_being_pressed())
					{
						g_logger.log(soup::ObfusString("F1 or F9 pressed, aborting."));
						load_aborted = true;
						break;
					}
					soup::os::sleep(100);
				}
			}

			__try
			{
				if (!mainInitPointers())
				{
					load_aborted = true;
				}
			}
			__EXCEPTIONAL()
			{
				load_aborted = true;
			}

			if (!load_aborted)
			{
				if (!early_inject)
				{
					SetForegroundWindow(pointers::hwnd);
				}
				__try
				{
					mainAfterPointers();
				}
				__EXCEPTIONAL()
				{
				}
#if ENABLE_PASSIVE_DLL
				if (!g_this_dll_is_passive)
#endif
				{
					std::vector<DetourHook*> hooks{};
					Components::collectHooks(hooks);
					for (const auto& hook : hooks)
					{
						if (hook->isHooked() && hook->isLongjump())
						{
							hook->removeHook();
						}
					}

					g_hooking.deinit();
				}
			}

			if (patched_is_explosion_type_valid)
			{
#if ENABLE_PASSIVE_DLL
				*pointers::is_explosion_type_valid_patch = (g_game_has_passive_dll ? CS_PASSIVE : CS_NONE);
#else
				*pointers::is_explosion_type_valid_patch = CS_NONE;
#endif
			}
			mainUnpatch();
			mainStopCountedThreads();
		}
		__EXCEPTIONAL()
		{
		}
		PeHeader::dispose();
#if ENABLE_PASSIVE_DLL
		if (g_this_dll_is_passive)
		{
			g_logger.log(soup::ObfusString("That's all, folks! Going passive."));

			for (lang_t i = 0; i != LANG_SIZE; ++i)
			{
				if (i != Lang::active_id)
				{
					Lang::id_to_map(i)->clear();
				}
			}

			mainDeinit();

			while (true)
			{
				if constexpr (AntiDbg::isEnabled())
				{
					if (AntiDbg::getRunningDebuggerId(true) != 0)
					{
						if (!g_logger.isInited())
						{
							g_logger.init(FileLogger::getMainFilePath());
						}
						AntiDbg::apologiseInAdvance(3);
						AntiDbg::crash();
					}
				}
				soup::os::sleep(500);
			}
		}
		else
#endif
		{
			Exceptional::disableExceptionHandling();
			mainUnload();
			FreeLibraryAndExitThread(g_hmodule, 0);
		}
	}

	void mainDeinit()
	{
		if (PointerCache::isInited())
		{
			//g_logger.log(soup::ObfusString("Weird, PointerCache is still inited at deinit time, flushing it out now.").str());
			PointerCache::saveAndDeinit();
		}

		// In case module does not get properly ejected, clean up resources now.
		g_gui.pseudo_commands.reset();
		g_gui.root_list.reset();
		g_gui.m_active_list.clear();
		g_gui.meta_state.data.clear();
		g_gui.active_profile.data.clear();
		g_gui.hotkeys.data.clear();
		g_gui.starred_commands.data.clear();
		joaatToStringDeinit();
		ResourceMgr::deinit();
		Lang::deinit();
		g_script_mgr.deinit();

		// Sanity check
		if (pointers::FormatInt64ToCash_mov_bAddCurrencyDeliminator
			&& *pointers::FormatInt64ToCash_mov_bAddCurrencyDeliminator != 0x44
			)
		{
			g_logger.log(soup::ObfusString("WTF, thousands separator patch was not undone?!"));
		}

		// Clean up everything related to logging up now.
		g_mystackwalker_inst.reset();
		g_logger.deinit();
		if (g_console.isInited())
		{
			g_console.deinit();
		}
	}

	void mainUnload()
	{
		g_logger.log(soup::ObfusString("It's time for me to go."));

#if HIDE_MODULE
		g_module_hider.disable();
#endif

		for (lang_t i = 0; i != LANG_SIZE; ++i)
		{
			Lang::id_to_map(i)->clear();
		}

		mainDeinit();

		SetUnhandledExceptionFilter(g_og_unhandled_exception_filter);

		DWORD OldProtect;
		for (size_t i = 0; i < g_stand_dll_range.size; i += 0x1000)
		{
			VirtualProtect(g_hmodule, 0x1000, PAGE_EXECUTE_READWRITE, &OldProtect);
		}

		uint8_t thread_asm[] = {
			/*  0 */ 0x55, // push rbp
			/*  1 */ 0x48, 0x89, 0xe5, // mov rbp, rsp
			/*  4 */ 0x48, 0x83, 0xec, 0x20, // sub rsp, 0x20

			/*  8 */ 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, (8 bytes)
			/* 18 */ 0x48, 0xC7, 0xC1, 0x0A, 0x00, 0x00, 0x00, // mov rcx, 10
			/* 25 */ 0xFF, 0xD0, // call rax

			/* 27 */ 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, (8 bytes)
			/* 37 */ 0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rcx, (8 bytes)
			/* 47 */ 0x31, 0xD2, // xor edx, edx
			/* 50 */ 0x49, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov r8, (8 bytes)
			0xFF, 0xD0, // call rax

			0x48, 0x83, 0xc4, 0x20, // add rsp, 0x20
			0x5d, // pop rsb
			0xc3, // retn
		};
		// oh no, we're leaking 67 bytes of memory :crybaby:
		auto area = reinterpret_cast<uint8_t*>(VirtualAlloc(nullptr, sizeof(thread_asm), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
		memcpy(area, thread_asm, sizeof(thread_asm));

		*(void**)(&area[8 + 2]) = &Sleep;
		*(void**)(&area[27 + 2]) = &memset;
		*(void**)(&area[37 + 2]) = g_hmodule;
		*(size_t*)(&area[50 + 2]) = g_stand_dll_range.size;

		HANDLE hThread = CreateRemoteThread(nullptr, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(area), nullptr, 0, nullptr);
		if (hThread)
		{
			Exceptional::hideThread(hThread);
			CloseHandle(hThread);
		}
	}

	static void mainInitLang()
	{
		wchar_t locale_name[LOCALE_NAME_MAX_LENGTH] = { 0 };
		GetUserDefaultLocaleName(locale_name, LOCALE_NAME_MAX_LENGTH); // Returns the locale used for "Regional format" for whatever reason
		auto user_lang_id = Lang::code_to_id(StringUtils::utf16_to_utf8(locale_name).substr(0, 2));
		if (Lang::isEnabled(user_lang_id))
		{
			Lang::default_id = user_lang_id;
		}

		if (auto lang_i = g_gui.active_profile.data.find("Stand>Settings>Language"); lang_i != g_gui.active_profile.data.end())
		{
			if (auto lang_id = Lang::name_to_id(lang_i->second); lang_id != LANG_SIZE)
			{
				return Lang::setActive(lang_id);
			}
		}
		return Lang::setActive(Lang::default_id);
	}

	static void mainProcessEarlyInject()
	{
		if (MapUtil::hasKeyValue(g_gui.active_profile.data, soup::ObfusString("Game>Disable Keyboard Hook").str(), soup::ObfusString("On").str()))
		{
			g_logger.log(soup::ObfusString("Disable Keyboard Hook is enabled in state, doing that now."));
			if (pointers::remove_keyboard_hook)
			{
				pointers::remove_keyboard_hook();
			}
		}
	}

	static void mainSetNotRunning()
	{
		g_running = false;
		g_gui.unload_state = UnloadState::HAND_OVER_TO_MAIN;
	}

	static void mainDisableHooks()
	{
		TimedCall::log(soup::ObfusString("Disabled all hooks in {}."), []
		{
			std::vector<DetourHook*> hooks{};
			Components::collectHooks(hooks);
			for (const auto& hook : hooks)
			{
				if (hook->isHooked() && hook->isLongjump())
				{
					hook->disableHook();
				}
			}

			g_hooking.disableAll();
		});
	}

#define LOG_NATIVE_REG_ORDER false

#if LOG_NATIVE_REG_ORDER
	[[nodiscard]] static SOUP_FORCEINLINE std::vector<rage::scrNativeHash> captureNativesSnapshot()
	{
		return pointers::native_registration_table->getKeys();
	}
#endif

	void mainAfterPointers()
	{
		bool processed_early_inject = false;

		if (early_inject)
		{
			mainProcessEarlyInject();
			processed_early_inject = true;
		}

		if (pointers::native_registration_table->initialised == false)
		{
			if (!processed_early_inject)
			{
				mainProcessEarlyInject();
				processed_early_inject = true;
			}

			if (!MapUtil::hasKeyValue(g_gui.active_profile.data, soup::ObfusString("Game>Early Inject Enhancements>Speed Up Startup").str(), soup::ObfusString("Off").str()))
			{
				if (pointers::native_scramble_count)
				{
					*pointers::native_scramble_count = 1;
				}
			}

			g_logger.log(soup::ObfusString("Waiting for game to initialise scripts."));

#if LOG_NATIVE_REG_ORDER
			time_t finished_at = 0;
			std::vector<std::vector<rage::scrNativeHash>> snapshots{};
			const std::vector<rage::scrNativeHash>* last_snapshot = &snapshots.emplace_back(captureNativesSnapshot());
			do
			{
				auto snapshot = captureNativesSnapshot();
				if (snapshot.size() != last_snapshot->size())
				{
					last_snapshot = &snapshots.emplace_back(std::move(snapshot));
				}
				if (finished_at == 0 && pointers::native_registration_table->initialised)
				{
					finished_at = get_current_time_millis();
				}
			} while (finished_at == 0 || GET_MILLIS_SINCE(finished_at) < 2000);

			std::unordered_set<rage::scrNativeHash> natives_heap{};
			for (const auto& snapshot : snapshots)
			{
				std::vector<rage::scrNativeHash> new_natives{};
				for (const auto& native : snapshot)
				{
					if (!natives_heap.contains(native))
					{
						natives_heap.emplace(native);
						new_natives.emplace_back(native);
					}
				}
				if (!new_natives.empty())
				{
					std::string msg{};
					for (const auto& native : new_natives)
					{
						StringUtils::list_append(msg, Util::to_padded_hex_string(native));
					}
					g_logger.log(std::move(msg));
				}
			}
#else
			do
			{
				soup::os::sleep(100);
			} while (pointers::native_registration_table->initialised == false
				&& !is_special_key_being_pressed()
				);

			if (pointers::native_scramble_count)
			{
				*pointers::native_scramble_count = 1000;
				pointers::native_scramble_count = nullptr;
			}

			if (is_special_key_being_pressed())
			{
				g_logger.log(soup::ObfusString("F1 or F9 pressed, aborting."));
				return;
			}
#endif
		}

		g_script_mgr.initEntrypointMaps();

		mainInitLang();

		joaatToStringInit();

		g_hooking.setTargets();

		if (!g_windows_7_or_older)
		{
			DiscordEventHandlers handlers;
			memset(&handlers, 0, sizeof(handlers));
			Discord_Initialize("842884721255251989", &handlers, false, nullptr);
		}

		g_auth.discoverActivationKey();
		if (g_auth.activation_key_to_try.empty())
		{
			g_gui.initial_auth_done.fulfil();
		}

		g_script_mgr.addScript(g_hmodule, std::make_unique<Script>(&Gui::scriptFunc));
		NativeTableHooksBuiltin::init();

		g_hooking.spoofed_name = pointers::rlPresence_GamerPresences_0_GamerInfo->name;

		auto hooks = g_hooking.getMinimalHooks();
		try
		{
#if ENABLE_PASSIVE_DLL
			if (!g_game_has_passive_dll)
			{
				g_hooking.mergeHooks(hooks, g_hooking.getPassiveHooks());
			}
#endif
			g_hooking.batchCreateHooks(hooks);
		}
		catch (const std::exception& e)
		{
			showMessageBoxAndLog(e.what());
			return;
		}

		{
			std::filesystem::path dohfile = get_appdata_path(soup::ObfusString("doh.txt"));
			if (std::filesystem::is_regular_file(dohfile))
			{
				g_logger.log(soup::ObfusString("Found doh.txt in Stand folder; moving this to Meta State as \"DNS Conduit: Smart\"."));
				g_gui.meta_state.data.emplace(soup::ObfusString("DNS Conduit"), soup::ObfusString("Smart"));
				g_gui.meta_state.save();
				std::filesystem::remove(dohfile);
			}
		}

		if (auto e = g_gui.meta_state.data.find(soup::ObfusString("DNS Conduit")); e != g_gui.meta_state.data.end())
		{
			switch (rage::atStringHash(e->second))
			{
			case ATSTRINGHASH("OS"):
				g_logger.log(soup::ObfusString("DNS Conduit set to OS."));
				HttpRequestBuilder::dns_conduit = DNS_CONDUIT_OS;
				break;

			case ATSTRINGHASH("SMART"):
				/*g_logger.log(soup::ObfusString("DNS Conduit set to Smart."));
				HttpRequestBuilder::dns_conduit = DNS_CONDUIT_SMART;*/
				break;

			case ATSTRINGHASH("HTTP"):
				g_logger.log(soup::ObfusString("DNS Conduit set to HTTP."));
				HttpRequestBuilder::dns_conduit = DNS_CONDUIT_HTTP;
				break;

			case ATSTRINGHASH("UDP"):
				g_logger.log(soup::ObfusString("DNS Conduit set to UDP."));
				HttpRequestBuilder::dns_conduit = DNS_CONDUIT_UDP;
				break;

			default:
				g_logger.log(soup::ObfusString("Unrecognised DNS Conduit; using default."));
				break;
			}
		}

		g_net_interface.updateConfig([](soup::netConfig& conf, soup::Capture&&) SOUP_EXCAL
		{
			HttpRequestBuilder::updateNetConfig(conf);
		});

		g_running = true;

#if false
		if (GetModuleHandleA(soup::ObfusString("ScriptHookV.dll")) != nullptr)
		{
			PatternBatch batch;
			{
				SIG_INST("48 8B 05 ? ? ? ? 83 38 00 74 3A 48 8D 0D");
				batch.add("A5", soup::Module("ScriptHookV.dll"), sig_inst, [](soup::Pointer p)
				{
					auto jz = p.add(10).as<uint8_t*>();

					DWORD oldProtect;
					VirtualProtect(jz, 1, PAGE_EXECUTE_READWRITE, &oldProtect);

					*jz = soup::Bytepatch::JMP1; // jz -> jmp
				});
			}
			batch.runSingleThread();

			if (batch.error_message.empty())
			{
				g_logger.log(soup::ObfusString("ScriptHookV.dll detected; disabled its Online check."));
			}
		}
#endif

#if true
		// This somewhat disables arxan related stuff so some functions can be hooked without crashing.
		{
			SIG_INST("48 8D 45 ? 48 89 45 ? 48 8D 05 ? ? ? ? 48 89 45");
			soup::Pointer results[1000];
			size_t res = g_gta_module.range.scanWithMultipleResults(std::move(sig_inst), results);
#ifdef STAND_DEBUG
			if (!has_game_finished_loading())
			{
				g_logger.log(fmt::format("Found {} arxan stubs.", res));
			}
#endif
			for (size_t i = 0; i != res; ++i)
			{
				memset(results[i].add(8).as<void*>(), 0x90, 7);
			}
		}
#endif

		std::string enabled_hooks_prefix{};

		if (!has_game_finished_loading())
		{
			if (!processed_early_inject)
			{
				mainProcessEarlyInject();
				processed_early_inject = true;
			}

			g_logger.log(soup::ObfusString("Doing minimal init."));
			g_hooking.batchEnableHooks(hooks);
			hooks.clear();
			did_minimal_init = true;
			g_gui.minimal = true;
			g_gui.initMinimal();
			mainRunningLoop();
			g_gui.minimal = false;
			if (is_special_key_being_pressed())
			{
				g_logger.log(soup::ObfusString("F1 or F9 pressed, aborting."));
				mainSetNotRunning();
				mainDisableHooks();
				return;
			}
			enabled_hooks_prefix = soup::ObfusString("Game has finished loading. ").str();
		}

		g_hooking.rage_fwConfigManager_GetSizeOfPool_hook.disableHook();
		g_hooking.rage_fwConfigManager_GetSizeOfPool_hook.removeHook();

#if BLOCK_ALL_METRICS
		// Set telemetry submission interval to 0 to disable telemetry submission.
		// Although we handle this in HTTP hook, we'd still trip minority report.
		*pointers::s_Policies_m_SubmissionIntervalSeconds = 0;
		pointers::s_Policies_m_SubmissionIntervalSeconds = nullptr;
#endif

		try
		{
			enabled_hooks_prefix.append(soup::ObfusString("Did main hooks in {}.").str());
			TimedCall::log(std::move(enabled_hooks_prefix), [&]
			{
				auto main = g_hooking.getMainHooks();
				g_hooking.batchCreateHooks(main);
				hooks.insert(hooks.end(), main.begin(), main.end());
				g_hooking.batchEnableHooks(hooks);
				g_hooking.hookWndproc();
			});
		}
		catch (const std::exception& e)
		{
			showMessageBoxAndLog(e.what());
			return;
		}

		g_hooking.performChecks();

		mainRunningLoop();

		SOUP_IF_UNLIKELY (g_gui.unload_state != UnloadState::HAND_OVER_TO_MAIN)
		{
			g_logger.log(soup::ObfusString("Unloading in an unusual state. Attempting high-level cleanup..."));
			g_gui.backToNormalNonScript();
			soup::os::sleep(1'000);
			uint8_t time_fallback_unload = 0;
			while (true)
			{
				soup::os::sleep(100);
				if (g_gui.unload_state == UnloadState::HAND_OVER_TO_MAIN)
				{
					g_logger.log(soup::ObfusString("Managed to do high-level cleanup."));
					break;
				}
				if (++time_fallback_unload == 100)
				{
					g_logger.log(soup::ObfusString("This is taking too long. Fine, we're doing it the ugly way..."));
					break;
				}
			}
		}

		mainSetNotRunning();

		g_logger.log(soup::ObfusString("Performing low-level cleanup..."));

		g_mystackwalker_inst.reset();

#if ENABLE_PASSIVE_DLL
		if (g_game_has_passive_dll)
#endif
		{
			mainDisableHooks();
		}
#if ENABLE_PASSIVE_DLL
		else
		{
			TimedCall::log(soup::ObfusString("Switched hooks in {}."), []
			{
				g_game_has_passive_dll = true;
				g_this_dll_is_passive = true;
				auto hooks = g_hooking.getNonPassiveHooks();
				g_hooking.batchDisableAndRemoveHooks(hooks);
			});
		}
#endif
		//g_script_mgr.remove_scripts(g_hmodule);

		//mainUnpatch(); // will be done later down the line in mainThread, pretty sure we dont't need to do it twice.

		if (!g_windows_7_or_older)
		{
			Discord_Shutdown();
		}

		g_renderer.releaseDevices();

		g_logger.log(soup::ObfusString("Stopping threads..."));

		if (g_relay.thread_running)
		{
			g_relay.close();
			uint8_t counter = 0;
			do
			{
				soup::os::sleep(100);
				if (IS_TAKING_TOO_LONG(20))
				{
					TerminateThread(g_relay.thread, 1);
					break;
				}
				if (counter == 3)
				{
					g_logger.log(soup::ObfusString("#1 seems to be holding us up."));
				}
			} while (g_relay.thread_running);
		}
		CloseHandle(g_relay.thread);

		{
			uint8_t counter = 0;
			while (g_gui.memory_thread_running)
			{
				soup::os::sleep(100);
				if (IS_TAKING_TOO_LONG(30))
				{
					TerminateThread(g_gui.memory_thread, 1);
					break;
				}
				if (counter == 8)
				{
					g_logger.log(soup::ObfusString("#2 seems to be holding us up."));
				}
			}
		}
		CloseHandle(g_gui.memory_thread);

		{
			uint8_t counter = 0;
			while (g_gui.worker_thread_running)
			{
				soup::os::sleep(100);
				if (IS_TAKING_TOO_LONG(40))
				{
					TerminateThread(g_gui.worker_thread, 1);
					break;
				}
				if (counter == 3)
				{
					g_logger.log(soup::ObfusString("#3 seems to be holding us up."));
				}
			}
		}
		CloseHandle(g_gui.worker_thread);

		if (g_gui.metrics_thread_running)
		{
			// Just shoot the metrics thread, can't be arsed to wait up to 1 second.
			TerminateThread(g_gui.metrics_thread, 1);
		}
		CloseHandle(g_gui.metrics_thread);

		mainStopCountedThreads();

		g_logger.log(soup::ObfusString("Tying up loose ends..."));

		if (Matchmaking::isRequestActive())
		{
			uint8_t counter = 0;
			do
			{
				soup::os::sleep(100);
				if (IS_TAKING_TOO_LONG(20))
				{
					break;
				}
				if (counter == 15)
				{
					g_logger.log(soup::ObfusString("#5 seems to be holding us up."));
				}
			} while (Matchmaking::isRequestActive());
		}

		if (!RemoteStats::shutdown())
		{
			uint8_t counter = 0;
			do
			{
				soup::os::sleep(100);
				if (IS_TAKING_TOO_LONG(20))
				{
					break;
				}
				if (counter == 15)
				{
					g_logger.log(soup::ObfusString("#6 seems to be holding us up."));
				}
			} while (!RemoteStats::shutdown());
		}

		if (RageConnector::isRequestActive())
		{
			g_logger.log(soup::ObfusString("#7 seems to be holding us up."));
			soup::os::sleep(15'000);
		}

#if I_CAN_SPY
		RageConnector::stopAllSpies();
#endif

		PubSubClient::unsubscribeAll();
		if (g_net_interface.isActive())
		{
			g_net_interface.setDontMakeReusableSockets();
			g_net_interface.closeReusableSockets();
			uint8_t counter = 0;
			do
			{
				soup::os::sleep(10);
				if (IS_TAKING_TOO_LONG(5))
				{
					g_logger.log(soup::ObfusString("#8 seems to be holding us up."));
#ifdef STAND_DEBUG
					for (const auto& w : g_net_interface.workers)
					{
						g_logger.log(fmt::format("- {}", w->toString()));
					}
#endif
					counter = 0;
				}
			} while (g_net_interface.isActive());
		}

		// keeping g_hooking alive for a bit avoid potential nullptr accesses
		soup::os::sleep(50);
		/*const char* script = g_hooking.executing_script_name;
		if (script != nullptr)
		{
			//g_logger.log(fmt::format("Waiting for the slow ass {} script to finish...", script));
			uint8_t counter = 0;
			do
			{
				soup::os::sleep(10);
				if (IS_TAKING_TOO_LONG(10))
				{
					g_logger.log(fmt::format("{} script seems to be holding us up.", script));
					//break;
				}
			}  while (g_hooking.executing_script_name != nullptr);
		}*/

		g_logger.log(soup::ObfusString("I can see the light..."));
	}

	bool mainInitPointers()
	{
		// Ensure game has window
		std::wstring sGrcWindow = StringUtils::utf8_to_utf16(soup::ObfusString("grcWindow").str());
		pointers::hwnd = FindWindowW(sGrcWindow.c_str(), nullptr);
		if (!pointers::hwnd)
		{
			std::string msg = soup::ObfusString("Failed to find the game's window.").str();
			showMessageBoxAndLog(msg.c_str());
			return false;
		}

		// Get window title
		const auto window_title_len = GetWindowTextLength(pointers::hwnd);
		std::string window_title(window_title_len, L'\0');
		GetWindowTextA(pointers::hwnd, &window_title[0], window_title_len + 1);

		// Init PointerCache
		if (!PointerCache::isInited())
		{
			PointerCache::init();
		}

		PatternBatch batch(&PointerCache::cache);

		// A - Startup
		// B - Basics
		// C - Chat
		// D - DirectX
		// F - Features
		// G - Game Bug Fixes
		// H - Patches
		// I - Input
		// L - Labels
		// N - Anti-Anti-Cheat
		// O - Custom DLCs
		// P - Protections
		// Q - Netcode
		// R - ROS
		// S - Spoofing & Scaleforms
		// V - Visuals
		// X - Crash Prevention
		// Y - Overflow Prevention
		// Z - Debug Build

		BATCH_ADD_OPTIONAL("A4", "48 8D 05 ? ? ? ? BE E8 03 00 00 48 89 83 40 01 00 00 48 8D 05", [](soup::Pointer p)
		{
			pointers::native_scramble_count = p.add(8).as<uint32_t*>();
		});

		BATCH_ADD("B0", "4C 8D 05 ? ? ? ? 48 8D 15 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 4C 24 20 E8", [](soup::Pointer p)
		{
			pointers::online_version = p.add(3).rip().as<const char*>();
			pointers::build_version = p.sub(165).rip().as<const char*>();
		});
		BATCH_ADD("B1", "8B 05 ? ? ? ? 85 C0 0F 84 ? ? ? ? 83 F8 06 74 08 83 C0 F8", [](soup::Pointer p)
		{
			pointers::CLoadingScreens_ms_Context = p.add(2).rip().as<int32_t*>();
		});
		BATCH_ADD("B2", "40 38 35 ? ? ? ? 75 0E 4C 8B C3 49 8B D7 49 8B CE", [](soup::Pointer p)
		{
			pointers::is_session_started = p.add(3).rip().as<bool*>(); // CNetwork::sm_bGameInProgress
		});
		BATCH_ADD("B3", "48 8B 05 ? ? ? ? 48 83 78 08 00 74 EB 48 8B 0D", [](soup::Pointer p)
		{
			pointers::ped_factory = p.add(3).rip().as<CPedFactory**>();
			pointers::network_player_mgr = p.add(17).rip().as<CNetworkPlayerMgr**>();
		});
		/*BATCH_ADD("B3", "50 ? 48 8B 42 ? 48 85", [](soup::Pointer p)
		{
			pointers::ped_factory = p.sub(9).rip().as<CPedFactory**>();
		});
		BATCH_ADD("B4", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", [](soup::Pointer p)
		{
			pointers::network_player_mgr = p.add(3).rip().as<CNetworkPlayerMgr**>();
		});*/
		BATCH_ADD("B4", "48 89 5C 24 ? 57 48 83 EC 20 F3 0F 10 05 ? ? ? ? 48 8B FA 48 8B D9 0F 2F 41 44", [](soup::Pointer p)
		{
			pointers::CExplosionEvent_Trigger = p.as<CExplosionEvent_Trigger_t>();
		});
		BATCH_ADD("B5", "84 C0 74 14 48 8B CF E8 ? ? ? ? 48 8B CF", [](soup::Pointer p)
		{
			p = p.add(8).rip(); // now within rage::rlPresence::SetActingGamerInfo
			pointers::rlPresence_m_ActingGamerInfo = p.add(6).rip().as<rage::rlGamerInfo*>();
		});
		BATCH_ADD("B6", "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A", [](soup::Pointer p)
		{
			pointers::native_registration_table = p.add(3).rip().as<rage::scrNativeRegistrationTable*>();
			//pointers::get_native_handler = p.add(12).rip().as<get_native_handler_t>();
		});
		BATCH_ADD_OPTIONAL_FUNC("B7", register_file, "48 89 5C 24", "48 89 6C 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC 50 48 8B EA 4C 8B FA 48 8B D9 4D 85 C9");
		BATCH_ADD("B8", "45 33 F6 8B E9 85 C9 B8", [](soup::Pointer p)
		{
			pointers::script_threads = p.sub(4).rip().sub(8).as<decltype(pointers::script_threads)>();
			p = p.sub(0x1F);
			STORE_HOOK(rage_scrThread_UpdateAll);
		});
		BATCH_ADD("B9", "42 88 84 3A ? ? ? ? 0F B6 05 ? ? ? ? 49 89 9C C7", [](soup::Pointer p)
		{
			p = p.add(19).rva(g_gta_module);
			pointers::script_programs_size = *p.sub(8).as<uint64_t*>();
			pointers::script_programs = p.as<rage::scrProgram**>();
		});
		BATCH_ADD("BA", "4C 8B D9 33 D2 4C 8D 15 ? ? ? ? 4C 8D 05", [](soup::Pointer p)
		{
			pointers::rage_scrProgram_sm_GlobalSizes = p.add(8).rip().as<int*>();
			pointers::rage_scrProgram_sm_Globals = p.add(15).rip().as<rage::scrValue**>();
		});
		BATCH_ADD("BC", "48 8B 0D ? ? ? ? 4C 8B CE E8 ? ? ? ? 48 85 C0 74 05 40 32 FF", [](soup::Pointer p)
		{
			pointers::script_handler_mgr = p.add(3).rip().as<CGameScriptHandlerMgr**>();
		});
		BATCH_ADD("BD", "48 8D 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 48 8D 0D ? ? ? ? 8A D8 E8 ? ? ? ? 84 DB 75 13 48 8D 0D", [](soup::Pointer p)
		{
			pointers::replay_interface = p.add(3).rip().as<CReplayInterfaceGame**>();
		});
		BATCH_ADD("BE", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 8B 15 ? ? ? ? 48 8B F9 48 83 C1 10 33 DB", [](soup::Pointer p)
		{
			pointers::CTheScripts_GetGUIDFromEntity = p.as<CTheScripts_GetGUIDFromEntity_t>();
			pointers::extension_id_script_guid = p.add(17).rip().as<uint32_t*>();
			pointers::script_guid_pool = p.add(0x2B).rip().as<rage::fwPool<rage::fwScriptGuid>**>();
		});
		// BF is unused
		BATCH_ADD("BH", "84 C0 75 ? 48 8B 07 49 8B D6 48 8B CF FF 50 ? 84 C0 74 ? 8B 57 ? 8B CD E8", [](soup::Pointer p)
		{
			pointers::create_and_send_pickup_rewards_event = p.add(26).rip().as<create_and_send_pickup_rewards_event_t>();
		});
		BATCH_ADD("BI", "F3 0F 10 ? ? ? ? ? 48 8B CE E8 ? ? ? ? 48 8B 4E", [](soup::Pointer p)
		{
			pointers::CVehicle_SetGravityForWheellIntegrator = p.add(12).rip().as<CVehicle_SetGravityForWheellIntegrator_t>();
		});
		BATCH_ADD("BJ", "44 38 33 75 30 66 44", [](soup::Pointer p)
		{
			pointers::find_object_by_id = p.sub(0x40).as<find_object_by_id_t>();
		});
		BATCH_ADD("BK", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 49 8B F0 8B EA 8B F9 E8", [](soup::Pointer p)
		{
			pointers::create_and_send_increment_stat_event = p.as<create_and_send_increment_stat_event_t>();
		});
		BATCH_ADD("BL", "48 8D 0D ? ? ? ? 66 44 89 05 ? ? ? ? E8 ? ? ? ? 0F B7 05 ? ? ? ?", [](soup::Pointer p)
		{
			p = p.add(3).rip();
			pointers::water_quads = p.as<WaterQuad**>();
			pointers::water_quads_size = p.add(8).as<uint16_t*>();
		});
		BATCH_ADD("BM", "B8 60 F0 FF FF 33 C9 C7 05 ? ? ? ? 94 11 00 00 89 05", [](soup::Pointer p)
		{
			pointers::world_x_max = p.add(9).rip().add(4).as<int32_t*>();
			pointers::world_x_min = p.add(19).rip().as<int32_t*>();
			pointers::world_y_min = p.add(25).rip().as<int32_t*>();
			pointers::world_y_max = p.add(31).rip().add(4).as<int32_t*>();
		});
		BATCH_ADD("BN", "74 1B 8B 15 ? ? ? ? 48 8D 4F 10 E8", [](soup::Pointer p)
		{
			pointers::entity_extension_id_head_blend_data = p.add(4).rip().as<uint32_t*>(); // has to be a pointer to handle stand possibly initialising before autoids are generated
		});
		BATCH_ADD("BO", "75 0D 80 3D ? ? ? ? 00 75 04 B0 01 EB 02", [](soup::Pointer p)
		{
			pointers::hide_hud_and_radar_this_frame = p.add(4).rip().add(1).as<bool*>();
		});
		BATCH_ADD("BP", "48 8B 0D ? ? ? ? 4C 8D 44 24 20 48 8B D6 E8 ? ? ? ? B0 01", [](soup::Pointer p)
		{
			pointers::network_session = p.add(3).rip().as<CNetworkSession**>();
			pointers::send_MsgTextMessage = p.add(16).rip().add(
				(0x00007FF7C1777D87 - 0x00007FF7C1777CF4) + 1
			).rip().as<send_MsgTextMessage_t>();
		});
		BATCH_ADD("BQ", "7C E7 48 8D 54 24 40 48 8D 0D", [](soup::Pointer p)
		{
			pointers::vehicle_metadata = p.add(10).rip().as<CVehicleMetadataMgr*>();
		});
		BATCH_ADD("BR", "75 17 48 8D 0D ? ? ? ? E8", [](soup::Pointer p)
		{
			pointers::local_peer_id = p.add(5).rip().as<rage::netPeerId*>();
			//pointers::get_peer_id = p.add(10).rip().as<void*>();
		});
		BATCH_ADD("BS", "48 8B E9 48 8B DA 49 8B CF E8 ? ? ? ? 48 8B 0D", [](soup::Pointer p)
		{
			pointers::voice_chat_data = p.add(17).rip().as<CNetworkVoice**>();
		});
		BATCH_ADD("BT", "74 77 0F B7 57 10 48 8B 0D ? ? ? ? 45 33 C0 E8", [](soup::Pointer p)
		{
			pointers::network_object_mgr = p.add(9).rip().as<CNetworkObjectMgr**>();
		});
		BATCH_ADD("BU", "F3 0F 10 52 10 F3 0F 10 02 8B 05", [](soup::Pointer p)
		{
			pointers::particle_fx_asset = p.add(11).rip().as<hash_t*>();
		});
		BATCH_ADD_OPTIONAL("BV", "48 8B D9 48 8B 0D ? ? ? ? 40 8A FA", [](soup::Pointer p)
		{
			pointers::ui3d_draw_mgr = p.add(6).rip().as<UI3DDrawManager**>();
		});
		BATCH_ADD("BW", "83 A3 A8 0F 00 00 00 83 A3 A4 0F 00 00 00 83 A3 A0 0F 00 00 00 48 8D 05", [](soup::Pointer p)
		{
			g_hooking.CEventGroupScriptNetwork_DispatchEvent_hook.setTarget(*p.add(24).rip().add(8).as<void**>());
		});
		BATCH_ADD("BX", "74 1D 4C 8D 05 ? ? ? ? 0F B7 C1", [](soup::Pointer p)
		{
			pointers::blips = p.add(5).rip().as<CMiniMapBlip**>();
		});
		BATCH_ADD("BY", "78 13 8B D1 48 8B 0D ? ? ? ? E8", [](soup::Pointer p)
		{
			pointers::cam_pool = p.add(7).rip().as<rage::fwPool<camBaseCamera>**>();
		});
		BATCH_ADD("BZ", "8B 15 ? ? ? ? 4C 8B 43 20 48 8B 05", [](soup::Pointer p)
		{
			pointers::handling_data_mgr = p.add(13).rip().as<CHandlingDataMgr*>();
		});
		// BG is unused

		BATCH_ADD("C0", "48 83 64 24 20 00 4C 8D 44 24 40 41 B9 01 00 00 00 E8", [](soup::Pointer p)
		{
			p = p.add(18).rip();
			CHECK_EXISTING_HOOK("C0", "48 89 5C 24 08");
			STORE_HOOK(send_CMsgTextMessage); // rage::netConnectionManager::Send<rage::CMsgTextMessage>
		});
		BATCH_ADD("C1", "48 8B EC 48 81 EC ? ? ? ? 4C 8B 11 48 8B 41 ? 48 83 65 C0 00 80 4D F8 01", [](soup::Pointer p)
		{
			p = p.sub(0x14);
			CHECK_EXISTING_HOOK("C1", "48 89 5C 24 08");
			STORE_HOOK(received_chat_message);
		});
		BATCH_ADD("C2", "48 8B 3D ? ? ? ? 48 8B 10 48 8B C8 0F 94 C3 FF 52 68", [](soup::Pointer p)
		{
			pointers::text_chat = p.add(3).rip().as<CNetworkTextChat**>();

			// Advance & rip for CNetworkTextChat_SubmitText
			p = p.add(32).rip();
			CHECK_EXISTING_HOOK("C2", "48 8B C4 48 89");
			STORE_HOOK(CNetworkTextChat_SubmitText);
		});
		BATCH_ADD("C3", "48 8B F1 B1 01 48 89", [](soup::Pointer p)
		{
			p = p.sub(0x00007FF7DA57F0F9 - 0x00007FF7DA57F0DC);
			CHECK_EXISTING_HOOK("C3", "40 55 56 57 41");
			STORE_HOOK(CMultiplayerChat_UpdateInputWhenTyping);
		});
		BATCH_ADD("C4", "74 04 C6 40 01 01 48 8B 0D", [](soup::Pointer p)
		{
			pointers::chat_box = p.add(9).rip().as<CMultiplayerChat**>();
		});
		BATCH_ADD("C5", "33 F6 33 C9 66 89 35 ? ? ? ? 66 89 35 ? ? ? ? 89 35", [](soup::Pointer p)
		{
			pointers::rage_ioKeyboard_sm_TextBuffer = p.add(7).rip().as<wchar_t*>();
		});
		BATCH_ADD_MANDATORY_HOOK("C6", scaleform_is_key_pressed_1, "44 3B 41 04 74", "0F 41 83 F8 1B 75 06 83 79 04 01 75 03 32 C0 C3 48 63 0D ? ? ? ? 48 63 C2 48 8D 15");
		BATCH_ADD_OPTIONAL_HOOK("C7", is_chat_character_valid, "40 55 48 8B EC", "48 83 EC 70 0F B7 D1 33 C9 33 C0 C7 45 90 7F 00 60 00");
		BATCH_ADD_OPTIONAL("C8", "48 8B D8 41 BC 1F 00 00 00", [](soup::Pointer p)
		{
			pointers::min_character_value = p.add(5).as<uint32_t*>();
		});
		BATCH_ADD("C9", "48 39 B3 58 01 00 00 75 24 48 8D 05", [](soup::Pointer p)
		{
			p = p.add(12).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("C9", "4D 85 C9 0F 84");
			STORE_POINTER(CMultiplayerChat_OnEvent);
		});
		BATCH_ADD("CA", "48 83 EC 20 8B 1D ? ? ? ? 48 8D 0D ? ? ? ? 8B D3", [](soup::Pointer p)
		{
			pointers::sm_systemTime_m_Time = p.add(6).rip().as<uint32_t*>();
		});
		BATCH_ADD("CB", "E8 ? ? ? ? 83 23 00 8B 4B 04", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			pointers::CMultiplayerChat_SetFocus = p.as<CMultiplayerChat_SetFocus_t>();
			pointers::rage_ioKeyboard_sm_AllowLocalKeyboardLayout = p.add((0x00007FF749923DE8 - 0x00007FF749923DA8) + 3).rip().as<bool*>();
		});
		BATCH_ADD("CC", "E8 ? ? ? ? 48 8D 43 4E", [](soup::Pointer p)
		{
			pointers::profanity_filter = p.sub(4).rip().as<rage::fwProfanityFilter*>();

			p = p.add(1).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("CC", "48 89 5C 24 08");
			STORE_POINTER(rage_fwProfanityFilter_AsteriskProfanity);
		});
		BATCH_ADD("CD", "45 33 F6 44 38 35 ? ? ? ? 41 8A DE", [](soup::Pointer p)
		{
			p = p.sub(0x00007FF749932B66 - 0x00007FF749932B48);
			CHECK_EXISTING_HOOK_WONT_HOOK("CD", "48 89 5C 24 10");
			STORE_POINTER(CNewHud_UpdateImeText);
		});

		BATCH_ADD("D0", "FF ? 41 8B DE EB 1B", [](soup::Pointer p)
		{
			p = p.add(14);
			CHECK_EXISTING_HOOK("D0", "44 8B C7 8B D3 48 8B 01 FF 50 40");
			pointers::swapchain = p.sub(4).rip().as<IDXGISwapChain**>();
			pointers::swapchain_present = p.sub(7).as<void*>();
			void* target = soup::Pointer(pointers::swapchain_present).followJumps().as<void*>();
			void* jumpout = p.as<void*>();
			if (target != pointers::swapchain_present)
			{
				jumpout = target;
			}
			swapchain_hook_init_present(
				&hooks::swapchain_present,
				jumpout
			);
		});
		BATCH_ADD_OPTIONAL("D1", "8B 44 24 50 4C 8B 17 44 8B 4E 04 44 8B 06 8B 54 24 68 48 8B CF C7 44 24 28 02 00 00 00 89 44 24 20 41 FF 52 68", [](soup::Pointer p)
		{
			swapchain_hook_init_resize_buffers(
				p.sub(4).rip().as<void*>(),
				&hooks::swapchain_resize_buffers,
				p.as<void*>()
			);
			pointers::swapchain_resize_buffers = p.sub(5).as<void*>();
		});
		{
			// CDXGISwapChain::PresentImpl
			SIG_INST("45 33 C0 44 89 74 24 20 48 8B CB E8");
			batch.add_optional("D2", soup::Module("dxgi.dll"), sig_inst, [](soup::Pointer p)
			{
				p = p.add(12).rip();
				STORE_HOOK(swapchain_present_streamproof);
			}, [](PatternBatch& batch)
			{
				SIG_INST("83 64 24 20 00 45 33 C0 48 8B CB E8");
				batch.add_optional("D3", soup::Module("dxgi.dll"), sig_inst, [](soup::Pointer p)
				{
					p = p.add(12).rip();
					STORE_HOOK(swapchain_present_streamproof);
				});
			});
		}
		BATCH_ADD("D4", "8B 05 ? ? ? ? 0F 44 05 ? ? ? ? 48 83 64 24 28 00 F3 0F 11 44 24 20 48 63 F0 48 8D 05", [](soup::Pointer p)
		{
			pointers::render_thread_interface = p.add(9).rip().sub(0x58).as<rage::fwRenderThreadInterface*>();
			pointers::viewport_manager = p.add(31).rip().sub(0x00).as<CViewportManager*>();
		});

		BATCH_ADD_OPTIONAL("F0", "48 83 EC 28 33 C9 FF 15 ? ? ? ? 45 33 C9 48 8D 15", [](soup::Pointer p)
		{
			pointers::add_keyboard_hook = p.as<void_func_no_args_t>();
			pointers::remove_keyboard_hook = p.add(38).rip().as<void_func_no_args_t>();
		});
		// F1 used by ComponentCrash
		BATCH_ADD_OPTIONAL_HOOK("F2", CControl_StartPlayerPadShake, "48 8B C4 48 89", "58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 30 83 B9 D0 1A 02 00 FF");
		BATCH_ADD_OPTIONAL("F3", "E8 ? ? ? ? 48 8B 4B 20 48 8B 11", [](soup::Pointer p)
		{
			pointers::CRagdollRequestEvent_Trigger = p.add(1).rip().as<CRagdollRequestEvent_Trigger_t>();
		});
		BATCH_ADD("F4", "E8 ? ? ? ? 44 8A E6 48 8B B3 ? ? ? ?", [](soup::Pointer p)
		{
			pointers::CAlterWantedLevelEvent_Trigger = p.add(1).rip().as<CAlterWantedLevelEvent_Trigger_t>();
		});
		BATCH_ADD_OPTIONAL_HOOK("F5", rage_FriendsReaderTask_Complete, "48 89 5C 24 10", "48 89 74 24 20 55 57 41 55 41 56 41 57 48 8D AC 24 60 FF FF FF 48 81 EC A0 01 00 00 48 8B F9 83 FA 03");
		BATCH_ADD("F6", "C6 44 24 20 01 E8 ? ? ? ? E9 ? ? ? ? 48 8B CB", [](soup::Pointer p)
		{
			pointers::CWeaponDamageEvent_Trigger = p.add(6).rip().as<CWeaponDamageEvent_Trigger_t>();
		});
		BATCH_ADD("F7", "E8 ? ? ? ? 8A 87 ? ? ? ? 88 46 0D", [](soup::Pointer p)
		{
			pointers::CRotaryWingAircraft_BreakOffTailBoom = p.add(1).rip().as<CRotaryWingAircraft_BreakOffTailBoom_t>();
		});
		BATCH_ADD("F8", "E8 ? ? ? ? 0F BA B7 ? ? ? ? ? 49 FF C6", [](soup::Pointer p)
		{
			pointers::CVehicleDamage_BreakOffWheel = p.add(1).rip().as<CVehicleDamage_BreakOffWheel_t>();
		});
		BATCH_ADD("F9", "E8 ? ? ? ? 84 C0 75 27 48 8B CE", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_POINTER(CPed_WantsToUseActionMode);
		});
		BATCH_ADD("FA", "E8 ? ? ? ? EB 08 83 8C 3E ? ? ? ? ?", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_POINTER(CPlaySoundEvent_TriggerPos);
			p = p.add(140);
			CHECK_EXISTING_HOOK("FA", "48 89 5C 24 08");
			STORE_POINTER(CPlaySoundEvent_TriggerEnt);
			p = p.add(0x00007FF638B809EB - 0x00007FF638B809D4).add(3).rip();
			STORE_POINTER(CNetwork_sm_EventMgr);
		});
		BATCH_ADD("FB", "E8 ? ? ? ? 0F BE 0B FF C6", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_POINTER(CGiveControlEvent_Trigger);
		});
		BATCH_ADD("FC", "E8 ? ? ? ? 44 8B 0D ? ? ? ? FF C3", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_POINTER(CProjectile_TriggerExplosion);
		});
#if false
		BATCH_ADD("FD", "E8 ? ? ? ? EB 08 66 41 0B CD", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_POINTER(CFireEvent_Trigger);
		});
#endif
		BATCH_ADD("FD", "C1 E8 0E 40 84 C7 75 5F", [](soup::Pointer p)
		{
			p = p.add(8);
			STORE_POINTER(CPedIntelligence_GetCanCombatRoll_patch);
		});
		BATCH_ADD("FE", "4C 8D 4F 28 48 89 74 24 20 E8", [](soup::Pointer p)
		{
			p = p.add(10).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("FE", "8B 05");
			STORE_POINTER(rage_rlProfileStats_ReadStatsByGamer);
		});
		BATCH_ADD("FF", "45 8B CF 44 8A C5 8B C8 48 8B D3 E8 ? ? ? ? E9", [](soup::Pointer p)
		{
			p = p.add(12).rip();
			STORE_POINTER(CAudioBankRequestEvent_Trigger);
		});
		BATCH_ADD("FG", "E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 65 48 8B 0C 25 ? ? ? ? 8B 05 ? ? ? ? BA ? ? ? ? 48 8B 04 C1 48 83 3C 02 ? 75 0C", [](soup::Pointer p)
		{
			p = p.add(1).rip().add(0x2A);
			STORE_POINTER(CTaskMotionInVehicle_CheckForThroughWindow_patch);
		});
		BATCH_ADD("FH", "48 8D 55 F0 48 8D 0D ? ? ? ? 48 98", [](soup::Pointer p)
		{
			pointers::gps_slots = p.add(7).rip().as<CGpsSlot*>();
		});
		BATCH_ADD("FI", "E8 ? ? ? ? 41 C6 45 ? ? FF C7", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_POINTER(CAircraftDamageBase_BreakOffSection);
		});
		BATCH_ADD("FJ", "E8 ? ? ? ? 83 25 ? ? ? ? ? 41 8A C6", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_POINTER(CNetworkPtFXEvent_Trigger);
		});
		BATCH_ADD_OPTIONAL_HOOK("FK", CPhysical_TestNoCollision, "48 89 5C 24 08", "48 89 74 24 ? 57 48 83 EC 20 4C 8B 41 50 33 DB");
		BATCH_ADD("FL", "E8 ? ? ? ? 48 8B 8C 1F ? ? ? ? 33 D2", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_POINTER(CStopSoundEvent_Trigger);
		});
		BATCH_ADD("FM", "E8 ? ? ? ? 49 8B CD 0F 28 F8", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			p = p.add(0x2C).add(4).rip();
			STORE_POINTER(balanced_cannons_time_between_shots);
		});
		BATCH_ADD("FN", "E8 ? ? ? ? 8B 15 ? ? ? ? 8B 0D ? ? ? ? 0F 2F", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			p = p.add(0x2C).add(4).rip();
			STORE_POINTER(balanced_cannons_alternate_wait_time);
		});
		BATCH_ADD("FO", "E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 89 7B 30", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_HOOK(rage_rlScMatchmaking_Advertise);
		});
		BATCH_ADD("FP", "E8 ? ? ? ? EB 21 B9 ? ? ? ?", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_HOOK(rage_rlScMatchmaking_Unadvertise);
		});
		BATCH_ADD("FQ", "E8 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8D 4D 28", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_HOOK(rage_netTransactor_SendResponse_rage_rlSessionDetailResponse);
		});

		BATCH_ADD_OPTIONAL_HOOK("G0", CProjectileManager_CreateProjectile, "48 89 5C 24 08", "89 54 24 10 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F9 48 81 EC B0 00 00 00");
		BATCH_ADD("G1", "48 63 0D ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 45", [](soup::Pointer p)
		{
			p = p.add(10).rip();
			STORE_POINTER(projectile_pool);
		});
		// G2 & G3 is used by ComponentSavedata

		BATCH_ADD_OPTIONAL("H0", "48 8B C8 FF 52 30 84 C0 74 05 48", [](soup::Pointer p)
		{
			pointers::model_spawn_bypass = p.add(8).as<uint8_t*>();
			ColoadMgr::addCheck("H0", p.as<void*>(), "\x48\x8B\xC8\xFF\x52\x30\x84\xC0\x74\x05\x48", 11);
		});
		BATCH_ADD("H1", "57 48 83 EC 20 8B D9 E8 ? ? ? ? 48 8B 10 48 8B C8", [](soup::Pointer p)
		{
			ColoadMgr::addCheck("H1", p.sub(5).as<void*>(), "\x48\x89\x5C\x24\x10", 5);
			pointers::is_explosion_type_valid_patch = p.add(0x00007FF7C108C8CA - 0x00007FF7C108C869).as<uint16_t*>(); // xor al, al
		});
		BATCH_ADD("H2", "0F 2F D0 73 05 0F 28 D0 EB 08 0F 2F D3 76 03 0F 28 D3 0F 2F C8 73 05 0F 28 C8 EB 08 0F 2F CB 76 03 0F 28 CB F3 0F 59 15 ? ? ? ? F3 0F 59 0D ? ? ? ? F3 0F 11 54 24 40 F3 0F 11 4C 24 44 E8 ? ? ? ? 48 8B 4C 24 40 48 89 88 E8 06 00 00 C6 80 FD 06 00 00 01 48 83 C4 28", [](soup::Pointer p)
		{
			pointers::clamp_gameplay_cam_pitch = p.sub(8).as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("H3", "C0 44 89 4C 24 20 E8", [](soup::Pointer p)
		{
			pointers::skybox_patch_ptr = p.add(0x7).rip().as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("H4", "3D 16 AA BE F8 74 18", [](soup::Pointer p)
		{
			pointers::lock_radio_lsur_hash = p.add(1).as<hash_t*>();
		});
		BATCH_ADD_OPTIONAL("H5", "37 87 59 E6", ([](soup::Pointer p)
		{
			pointers::radio_talk02_hash = p.as<hash_t*>();
		}));
		BATCH_ADD_OPTIONAL("H6", "41 83 F8 02 48 8B C8 0F 9E C2 E8", [](soup::Pointer p)
		{
			pointers::self_radio_patch.initPatchNOP(p.add(10).as<uint8_t*>(), 5);
		});
		BATCH_ADD("H7", "48 8B 11 0F 57 C0 0F 57 C9 8B 42 0C F3 48 0F 2A C0 8B 42 08 F3 48 0F 2A C8 F3 0F 5E C1 F3 0F 59", [](soup::Pointer p)
		{
			pointers::get_bink_movie_time_pre_ret = p.as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("H8", "84 C0 74 8B 48 8D 15", [](soup::Pointer p)
		{
			pointers::watchdog_thread_crash_patch.initPatchNOP(p.add(2).as<uint8_t*>(), 2);
		});
		BATCH_ADD_OPTIONAL("H9", "44 38 35 ? ? ? ? 0F 85 ? 00 00 00 48 8B 05", [](soup::Pointer p)
		{
			// fire_commands::CommandAddOwnedExplosion
			// set sm_BeenBusted to true so it won't try to do any funny shit anymore
			*p.add(3).rip().as<bool*>() = true;
		});
		BATCH_ADD_OPTIONAL("HA", "48 85 C0 0F 84 ? ? ? ? 8B 48 50", [](soup::Pointer p)
		{
			pointers::pseudo_object_check = p.as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("HB", "75 0C 33 C9 FF", [](soup::Pointer p)
		{
			pointers::hide_cursor_jnz = p.as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("HC", "8B D9 E8 ? ? ? ? 84 C0 75 6A 8B CB E8", [](soup::Pointer p)
		{
			pointers::request_control_spectating_check = p.add(2).as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("HD", "75 17 44 38 3D ? ? ? ? 74 12", ([](soup::Pointer p)
		{
			pointers::sp_ground_snow_check = p.as<uint8_t*>();
		}));
		BATCH_ADD("HE", "80 BC 1F ? ? ? ? 00 74 11 80 3D ? ? ? ? 00 74 08", [](soup::Pointer p)
		{
			pointers::stop_sound_dont_network = p.add(16).as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("HF", "48 85 DB 0F 84 D0 00 00 00 48 8B CB", [](soup::Pointer p)
		{
			pointers::ShouldPerformInVehicleHairScale_jz = p.add(3).as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("HG", "40 8A DF E8 ? ? ? ? 84 C0 74 11", [](soup::Pointer p)
		{
			pointers::UpdateTypers_call_IsInSpectatorMode = p.add(3).as<uint8_t*>();
		});
		BATCH_ADD_OPTIONAL("HI", "74 09 41 B5 01 44 88 6D E0", [](soup::Pointer p)
		{
			pointers::weaponwheel_update_isinvehicle_true = p.add(4).as<bool*>();
		});
		BATCH_ADD_OPTIONAL("HJ", "48 83 EC 38 44 88 4C 24 20 4C 8D 0D ? ? ? ? E8", [](soup::Pointer p)
		{
			pointers::FormatInt64ToCash_mov_bAddCurrencyDeliminator = p.add(4).as<uint8_t*>();

			p = p.add(17).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("HJ", "48 89 5C 24 10");
			STORE_POINTER(CTextConversion_FormatIntForHumans);
		});
		BATCH_ADD_OPTIONAL("HK", "C6 05 ? ? ? ? 00 F3 0F 10 35 ? ? ? ? 48 8D 05", [](soup::Pointer p)
		{
			pointers::rage_fwTimer_sm_aTimeScales = p.add(0x7 + 4).rip().as<float(*)[4]>();
			pointers::authoritative_time_scale_offset = p.add(0x3D + 4).as<uint32_t*>();
			pointers::rage_fwTimer_sm_fTimeScale = p.add(0x45 + 4).rip().as<float*>();
		});
		BATCH_ADD_OPTIONAL("HL", "3B CB 74 09 45 84 D2 0F 85", [](soup::Pointer p)
		{
			pointers::model_blacklist_patch.initPatchNOP(p.add(7).as<uint8_t*>(), 6);
		});
		BATCH_ADD_OPTIONAL("HM", "74 DA 0F B6 05 ? ? ? ? 3B F7 B9 01 00 00 00", [](soup::Pointer p)
		{
			pointers::rage__audRequestedSettings__sm_IgnoreBlockedAudioThread = p.add(5).rip().as<bool*>();
		});

		// rage::ioMapper::Update
		BATCH_ADD_OPTIONAL("I0", "0F 28 F7 83 FF 09", [](soup::Pointer p)
		{
			STORE_HOOK(set_value_from_keyboard);
			set_value_from_keyboard_init(p.add(6).as<void*>());
		});
		BATCH_ADD_OPTIONAL("I1", "8B C7 45 8A C2", [](soup::Pointer p)
		{
			STORE_HOOK(set_value_from_mkb_axis);
			set_value_from_mkb_axis_init_ret_og(p.add(5).as<void*>());
		});
		BATCH_ADD_OPTIONAL("I2", "49 8B 9C DE B8 15 00 00", [](soup::Pointer p)
		{
			set_value_from_mkb_axis_init_ret_bail(p.as<void*>());
		});

		BATCH_ADD_OPTIONAL_HOOK("L0", CTextFile_Get, "48 89 5C 24", "57 48 83 EC 20 48 8B DA 48 8B F9 48 85 D2 75 44 E8");
		BATCH_ADD("L1", "83 8B B0 02 00 00 FF 48 8D 8B", [](soup::Pointer p)
		{
			p = p.sub(0x00007FF6C3ABDEE0 - 0x00007FF6C3ABDEB8);
			CHECK_EXISTING_HOOK("L1", "48 89 5C 24 08");
			STORE_HOOK(CTextFile_GetInternal);
		});
		BATCH_ADD("L2", "48 8D 0D ? ? ? ? 33 D2 E8 ? ? ? ? 33 D2 40 88 2D", [](soup::Pointer p)
		{
			pointers::thetext = p.add(3).rip().as<CTextFile*>();
		});
		BATCH_ADD("L3", "E8 ? ? ? ? 48 8B 1D ? ? ? ? 44 8B C7 48 8B CB BA CE 00 00 00 E8", [](soup::Pointer p)
		{
			pointers::CPauseMenu_SetMenuPreference = p.add(1).rip().as<CPauseMenu_SetMenuPreference_t>();
			pointers::profile_settings = p.add(8).rip().as<CProfileSettings**>();
			pointers::CProfileSettings_Set = p.add(24).rip().as<CProfileSettings_Set_t>();
			pointers::rage_rlSetLanguage = p.add(47).rip().as<rage_rlSetLanguage_t>();
		});
		BATCH_ADD_FUNC("L4", CPauseMenu_UpdateProfileFromMenuOptions, "48 89 5C 24 08", "57 48 83 EC 20 48 8B 1D ? ? ? ? 44 8B 05");

		BATCH_ADD_OPTIONAL_HOOK("N0", rage_rlTelemetry_Export, "48 8B C4 48 89", "58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 30 49 8B E8 4C 8D 40 EC 49 8B F1 48 8B D9 40 32 FF E8");
#if HTTP_HOOK
		BATCH_ADD("N1", "48 8D 6C 24 30 48 89 9D 30 04 00 00 48 89 B5 40 04 00 00", [](soup::Pointer p)
		{
			p = p.sub(16);
			if (pointers::is_explosion_type_valid_patch != nullptr && *pointers::is_explosion_type_valid_patch != CS_PASSIVE)
			{
				CHECK_EXISTING_HOOK("N1", "40 55 57 41 54 41 55 41 57");
			}
			STORE_HOOK(rage_netHttpRequest_Update);
		});
#endif
		BATCH_ADD_MANDATORY_HOOK("N2", rage_netEventMgr_SendEvent, "48 89 5C 24 10", "48 89 4C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 81 EC 80 00 00 00 4C 8B 3D");
#if BLOCK_ALL_METRICS
		BATCH_ADD("N3", "83 3D ? ? ? ? 00 0F 86 ? ? ? ? 48 83", [](soup::Pointer p)
		{
			pointers::s_Policies_m_SubmissionIntervalSeconds = p.add(2).rip().add(1).as<uint32_t*>();
		});
#endif
#if CLEAR_BONUS_ON_DL
		BATCH_ADD("N4", "48 8B 0D ? ? ? ? 48 85 C9 75 05 41 8A F7 EB 14 44 8B 42 20 48 8B 52 18 E8", [](soup::Pointer p)
		{
			auto og = p;

			p = p.sub(0x7A);
			CHECK_EXISTING_HOOK("N4", "48 8B C4 48 89");
			STORE_HOOK(CTunables_OnCloudEvent);

			p = og;

			pointers::rage_AES_ms_cloudAes = p.add(3).rip().as<void**>();

			p = p.add(26).rip();
			STORE_POINTER(rage_AES_Decrypt);
		});
		BATCH_ADD("N5", "48 8B 53 ? 48 8D 4C ? ? E8 ? ? ? ? 48 8D 4C ? ? E8 ? ? ? ? B0 01 48 81 C4", [](soup::Pointer p)
		{
			p = p.add(10).rip();
			STORE_POINTER(rage_AES_Encrypt);
		});
#endif
#if PREVENT_PGSTREAMER_CRASH
		BATCH_ADD_OPTIONAL("N6", "89 15 ? ? ? ? 85 C0 0F 8E", ([](soup::Pointer p)
		{
			pointers::rage_pgStreamer_smTamperCrash = p.add(2).rip().sub(4).as<rage::sysObfuscated_Mutate<int>*>();

			uint8_t	patch[] = {
				0x90, 0xE9,
			};
			pointers::pgstreamer_crash_patch.initPatch(p.add(8).as<uint8_t*>(), patch, COUNT(patch));
		}));
#endif
#if PREVENT_GBUFFER_CRASH
		BATCH_ADD_OPTIONAL("N7", "8B 05 ? ? ? ? 4C 8B 05 ? ? ? ? 33 C9 85 C0 8B D1 0F 94 C2 48 0F AF", ([](soup::Pointer p)
		{
			pointers::GBuffer_m_Attached = p.add(2).rip().as<uint32_t*>();
			
			uint8_t patch[] = {
				0x31, 0xc0, // xor eax, eax
				0x90, 0x90, 0x90, 0x90
			};
			pointers::gbuffer_crash_patch.initPatch(p.as<uint8_t*>(), patch, COUNT(patch));
		}));
#endif
		BATCH_ADD_OPTIONAL("N8", "40 53 48 83 EC 20 48 8B 59 20 EB 0D 48 8B 03 48 8B CB FF 50 08 48 8B 5B 18", [](soup::Pointer p)
		{
			STORE_HOOK(rage_gameSkeleton_updateGroup_Update);
		});
		BATCH_ADD_OPTIONAL("N9", "E8 ? ? ? ? 44 8B B6 ? ? ? ? 48 8B CE", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_HOOK(network_bail);
		});
#if PREVENT_GRCDEVICE_CRASH
		BATCH_ADD_OPTIONAL("NA", "8B 05 ? ? ? ? A9 10 10 10 10 74", ([](soup::Pointer p)
		{
			pointers::rage_grcDevice_sm_KillSwitch = p.add(2).rip().as<uint32_t*>();

			uint8_t patch[] = {
				soup::Bytepatch::JMP1
			};
			pointers::grcdevice_crash_patch.initPatch(p.add(11).as<uint8_t*>(), patch, COUNT(patch));
		}));
#endif
		BATCH_ADD("NB", "48 8B 0D ? ? ? ? E8 ? ? ? ? 44 38 63 0C 74", [](soup::Pointer p)
		{
			p = p.add(8).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("NB", "40 53 48 83 EC");
			STORE_POINTER(CNetworkAssetVerifier_RefreshEnvironmentCRC);
		});
		// Can be found shortly before "BD C3 9E 26 00"
		BATCH_ADD("ND", "48 8B 15 ? ? ? ? 33 C9 48 39 0D", [](soup::Pointer p)
		{
			pointers::ac_thing_data = p.add(3).rip().as<AcThingData**>();
		});
#if false
		BATCH_ADD("NE", "48 8D 0D ? ? ? ? BA ? ? ? ? 74 05 BA ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? ? 48 8D 0D ? ? ? ? BA ? ? ? ? 84 DB 75 05 BA ? ? ? ? E8 ? ? ? ? 48 8B CD C6 05 ? ? ? ? ? E8 ? ? ? ? 84", [](soup::Pointer p)
		{
			pointers::game_skeleton = p.add(3).rip().as<rage::gameSkeleton*>();
		});
#endif
		BATCH_ADD_OPTIONAL_HOOK("NE", rage_sysDependencyScheduler_InsertInternal, "48 89 5C 24 08", "57 48 83 EC 20 0F B6 99 ? 00 00 00 4C 8B C1");
		BATCH_ADD("NF", "E8 ? ? ? ? 8B 54 24 30 89 13", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_HOOK(network_can_access_multiplayer);
		});

		BATCH_ADD_OPTIONAL("O1", "E8 ? ? ? ? 48 8B 53 20 44 8B C7 41 8B CE E8", [](soup::Pointer p)
		{
			const auto og = p;

			p = p.add(1).rip();
			CHECK_EXISTING_HOOK("O1", "81 E1 00 00 F0 0F");
			//STORE_HOOK(rage_AES_isTransformITKeyId);

			p = og.add(16).rip();
			CHECK_EXISTING_HOOK("O2", "48 8B C4 48 89");
			g_hooking.aes_initfile_hook.setTarget(og.as<void*>());
			aes_initfile_init(p.as<void*>(), og.add(20).as<void*>());
		});
		BATCH_ADD_OPTIONAL("O3", "E8 ? ? ? ? 8B 55 F8 48 8B 43 10 48 03 D0 48 8B CB 48 89 53 18 66 44 89 22 33 D2 E8", [](soup::Pointer p)
		{
			g_hooking.aes_decrypt_callsite_hook.setTarget(p.as<void*>());
			aes_decrypt_init(p.add(1).rip().as<void*>(), p.add(5).as<void*>());

			p = p.add(1).rip();
			CHECK_EXISTING_HOOK("O3", "48 89 5C 24 08");
		});
		BATCH_ADD("O5", "41 B0 01 E8 ? ? ? ? EB 05 E8 ? ? ? ? 48 8B 5C 24 30", [](soup::Pointer p)
		{
			const auto og = p;

			p = p.sub(103);
			CHECK_EXISTING_HOOK("O5", "48 89 5C 24 08");
			STORE_HOOK(CModelInfo_AddVehicleModel);

			p = og.add(11).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("O6", "48 89 5C 24 08");
			STORE_POINTER(rage_fwArchetypeManager_RegisterStreamedArchetype);
		});
		BATCH_ADD("O7", "48 8D 05 ? ? ? ? 48 8D BB ? 00 00 00 41 8D 48", [](soup::Pointer p)
		{
			p = p.sub(21);
			CHECK_EXISTING_HOOK_WONT_HOOK("O7", "48 89 5C 24 08");
			STORE_POINTER(CVehicleModelInfo_ctor);
		});
		BATCH_ADD("O8", "4C 8B C3 E8 ? ? ? ? 48 8D 1D ? ? ? ? 48 8D", [](soup::Pointer p)
		{
			p = p.sub(70);
			CHECK_EXISTING_HOOK_WONT_HOOK("O8", "48 8B C4 48 89");
			STORE_POINTER(CExtraContentManager_AddContentFolder);
		});
		BATCH_ADD("O9", "48 8B CE C6 80 F0 00 00 00 01 E8", [](soup::Pointer p)
		{
			p = p.add(11).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("O9", "48 89 5C 24 18");
			STORE_POINTER(CExtraContentManager_LoadContent);
		});
		BATCH_ADD_OPTIONAL("OA", "48 8B 89 50 FE FF FF 48 8B DA 48 8B 41 20", [](soup::Pointer p)
		{
			p = p.sub(13);
			CHECK_EXISTING_HOOK("OA", "48 89 5C 24 08");
			STORE_HOOK(CNetObjVehicle_GetVehicleCreateData);
		});
		// O4 is unused

		BATCH_ADD("P0", "89 44 24 28 8B 44 24 4C 4D 8B C6 49 8B D7 49 8B CC 89 44 24 20 E8", [](soup::Pointer p)
		{
			p = p.add(22).rip();
			STORE_HOOK(rage_netEventMgr_ProcessEventData);
		});
		BATCH_ADD("P1", "48 89 74 24 18 57 48 83 EC 20 48 8B D9 33 C9 41 8B F0", [](soup::Pointer p)
		{
			p = p.sub(5);
			CHECK_EXISTING_HOOK_WONT_HOOK("P1", "48 89 5C 24 10");
			pointers::read_bitbuffer_dword = p.as<read_bitbuffer_dword_t>();
		});
		BATCH_ADD("P2", "57 48 83 EC ? 41 8B ? 4C 8B ? 48 8B ? 45 85 ? 74", [](soup::Pointer p)
		{
			pointers::rage_datBitBuffer_ReadBits = p.sub(5).as<rage_datBitBuffer_ReadBits_t>();
		});
		BATCH_ADD_FUNC("P3", rage_datBitBuffer_WriteBits, "48 89 5C 24 08", "57 48 83 EC 30 F6 41 1C 01 45 8B D1");
		BATCH_ADD("P4", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 80 7A ? FF 41 8B F1 49 8B E8 48 8B DA 48 8B F9 74 59 0F B6 42 ? 44 8B 4C 24 ? 48 81 C1 ? ? ? ? 44 8B C6 48 8B D5 48 69 C0", [](soup::Pointer p)
		{
			pointers::send_net_event_ack = p.as<send_net_event_ack_t>();
		});
		BATCH_ADD("P5", "75 11 48 8B 89 D0 00 00 00 48 85 C9 74 05", [](soup::Pointer p)
		{
			p = p.sub(0x29);
			CHECK_EXISTING_HOOK_WONT_HOOK("P4", "48 83 EC 28 48");
			STORE_POINTER(rage_fwEntity_GetAttachedTo);
			// This function is actually entity_commands::GetEntityAttachParent which calls rage::fwEntity::GetAttachParent and CNetObjPhysical::GetEntityAttachedTo as a fallback.
		});
		BATCH_ADD_OPTIONAL_HOOK("P6", rage_netPlayerMgrBase_SendBuffer, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 60 48 8B 84 24 B0 00 00 00");
		BATCH_ADD_OPTIONAL("P7", "45 33 C9 4C 8B DA 66 85 C0 0F 84 ? ? ? ? 44 0F B7 C0 33 D2 8B C1 41 F7 F0 48 8B 05 ? ? ? ? 4C 8B 14 D0 EB 09 41 3B 0A 74 54", [](soup::Pointer p)
		{
			p = p.sub(7);
			CHECK_EXISTING_HOOK("P7", "0F B7 05");
			STORE_HOOK(rage_fwArchetypeManager_GetArchetypeFromHashKey);
		});
		BATCH_ADD("P8", "74 0C 48 8B D7 E8 ? ? ? ? 48 83 27 00", [](soup::Pointer p)
		{
			p = p.add(6).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("P8", "48 89 5C 24 08");
			STORE_POINTER(remove_known_ref);
		});
		BATCH_ADD_OPTIONAL("P9", "48 8B 54 24 68 49 8D 4C 24 60", [](soup::Pointer p)
		{
			p = p.add(5);
			STORE_HOOK(net_event_error_pre_memmove);
			net_event_error_init(p.add(5).as<void*>());
		});
		BATCH_ADD_FUNC("PA", CGameWorld_Remove, "48 89 5C 24 08", "48 89 74 24 10 57 48 83 EC 20 48 89 0D");
		BATCH_ADD("PB", "48 85 FF 48 89 1D", [](soup::Pointer p)
		{
			p = p.sub(4).rip();
			STORE_HOOK(script_vm);
			p = p.add(134);
			/* "PC" */ pointers::script_vm_switch = p.as<void*>();
			const auto jmp_table_p = p.add(27).rva(g_gta_module);
			for (uint8_t i = 0; i != pointers::script_vm_num_opcodes; ++i)
			{
				pointers::script_vm_jump_table[i] = jmp_table_p.add(i * 4).rva(g_gta_module).as<void*>();
			}
			script_vm_helper_init_1(pointers::script_vm_jump_table, &ScriptVmErrorHandling::onInvalidScriptOpCode);
			script_vm_op_native_init(&pointers::script_vm_jump_table[Native]);
		});
		BATCH_ADD_OPTIONAL("PD", "4D 85 C0 74 06 2B FE 41 89 78 14 45 89 57 08 41 8B C2 EB", [](soup::Pointer p)
		{
			script_vm_helper_init_2(&ScriptVmErrorHandling::onScriptThreadError, p.add(19).ripT<int8_t>().as<void*>());
			pointers::script_thread_error_kill_1 = p.add(11).as<void*>();
		});
		BATCH_ADD_OPTIONAL("PE", "B8 02 00 00 00 41 89 47 08 48 81 C4 B8 00 00 00", [](soup::Pointer p)
		{
			pointers::script_thread_error_kill_2 = p.as<void*>();
		});
		BATCH_ADD_OPTIONAL_HOOK("PF", received_clone_create_ack, "48 8b c4 48 89", "58 ? 48 89 68 ? 48 89 70 ? 48 89 78 ? 41 54 41 56 41 57 48 83 ec ? 4c 8b fa 49 8b d8");
		BATCH_ADD_OPTIONAL_HOOK("PG", received_clone_sync_ack, "48 89 5c 24", "48 89 74 24 ? 48 89 7c 24 ? 41 54 41 56 41 57 48 83 ec ? 4c 8b e2");
		BATCH_ADD_OPTIONAL_HOOK("PH", received_clone_remove_ack, "48 8b c4 48 89", "58 ? 48 89 68 ? 48 89 70 ? 57 41 56 41 57 48 83 ec ? 4c 8b fa 49 8b d8");
		BATCH_ADD_MANDATORY_HOOK("PI", received_clone_create, "48 8B C4 66 44", "89 48 20 4C 89 40 18 48 89 48 08 53 55 56 57 41 54 41 55 41 56 41 57 48 83 EC 68 44 0F B7 ? 24 D8 00 00 00");
		BATCH_ADD_MANDATORY_HOOK("PJ", received_clone_sync, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 40 4C 8B EA");
		BATCH_ADD_OPTIONAL_HOOK("PK", clone_pack, "48 89 5c 24", "48 89 6c 24 ? 48 89 74 24 ? 57 48 83 ec ? 48 8b f9 48 8b ca 49 8b e9");
		//BATCH_ADD_OPTIONAL_HOOK("PL", send_clone_create, "4C 8B DC 49 89", "5B 08 49 89 6B 18 49 89 73 20 57 41 54 41 55 41 56 41 57 48 83 EC 50 48 8B 02 4D 8B F8");
		BATCH_ADD_OPTIONAL_HOOK("PM", send_clone_sync, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 40 0F B6 72");
		//BATCH_ADD_OPTIONAL_HOOK("PN", send_clone_remove, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 30 41 80 78 ? FF 41 8A E9 49 8B F8 48 8B DA 48 8B F1 74");
		BATCH_ADD_OPTIONAL_HOOK("PO", rage_netSyncTree_Read, "48 8B C4 48 89", "58 08 48 89 68 10 48 89 70 20 44 89 40 18 57 48 83 EC 30 44 8B 05");
		BATCH_ADD("PP", "49 8B CE FF 90 A0 00 00 00 84 C0 74 31 33 FF", [](soup::Pointer p)
		{
			p = p.sub(0x2C);
			STORE_HOOK(sync_can_apply);
		});
		BATCH_ADD_OPTIONAL("PQ", "48 89 4C 24 20 48 8B 0D ? ? ? ? 48 8B D0 E8 ? ? ? ? F6 C3 02 74", [](soup::Pointer p)
		{
			p = p.add(16).rip();
			CHECK_EXISTING_HOOK("PQ", "48 8B C4 48 89");
			STORE_HOOK(parse_presence_event);
		});
		BATCH_ADD("PR", "4C 8D 15 ? ? ? ? 49 63 C0 48 8D 0C 40", [](soup::Pointer p)
		{
			p = p.add(3).rip();
			STORE_POINTER(pop_multiplier_areas);
		});
		BATCH_ADD("PS", "48 8B 0D ? ? ? ? 45 33 C0 49 8B D7 E8", [](soup::Pointer p)
		{
			p = p.add(14).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("PS", "48 89 5C 24 08");
			STORE_POINTER(CPedFactory_DestroyPed);
		});
		BATCH_ADD_FUNC("PT", vehicle_commands__DeleteScriptVehicle, "48 85 C9 74 38", "53 48 83 EC 20 80 89 ? ? 00 00 10 48 8B D9 48 8B 89 D0 00 00 00");
		BATCH_ADD("PU", "76 09 48 83 C4 28 E9 ? ? ? ? E8", [](soup::Pointer p)
		{
			p = p.add(12).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("PU", "48 8B C4 48 89");
			STORE_POINTER(CObjectPopulation_DestroyObject);
		});
		BATCH_ADD_OPTIONAL_HOOK("PV", CNetObjVehicle_SetVehicleControlData, "48 89 5C 24 10", "48 89 6C 24 18 48 89 74 24 20 57 41 54 41 55 41 56 41 57 48 83 EC 40 48 8B B1 50 FE FF FF 8A 9A");
		BATCH_ADD_OPTIONAL_HOOK("PW", rage_ropeManager_Remove, "48 89 5C 24 08", "48 89 74 24 10 57 48 83 EC 20 48 8B D9 48 8D 0D ? ? ? ? 48 8B FA E8 ? ? ? ? 48 8B 0F E8");
		BATCH_ADD_OPTIONAL("PX", "BA 6E 79 00 00 E8 ? ? ? ? 83 F8 01", [](soup::Pointer p)
		{
			// I'm pretty sure they meant to compare the bone index to -1 instead of 1...
			// Although, it should be fair to assume that a ped would always have a head bone.
			// This seems to mess up with head blend data on a non-online ped.
			uint8_t byte = -1;
			pointers::point_gun_at_head_patch.initPatch(p.add(12).as<uint8_t*>(), &byte, 1);
		});

		// Q0 is added by ComponentNetcode
		BATCH_ADD("Q1", "4D 8B C5 49 8B D7 49 8B CE 89 5C 24 20 E8", [](soup::Pointer p)
		{
			p = p.add(14).rip();
			CHECK_EXISTING_HOOK("Q1", "48 89 5C 24 18");
			STORE_HOOK(InviteMgr_AcceptInvite);
		});
		BATCH_ADD("Q2", "48 8B CB 48 89 44 24 28 83 64 24 20 00 E8", [](soup::Pointer p)
		{
			p = p.add(14).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("Q2", "48 89 5C 24 10");
			pointers::CNetworkSession_KickPlayer = p.as<CNetworkSession_KickPlayer_t>();
		});
		BATCH_ADD("Q3", "39 3D ? ? ? ? 0F 85 ? ? ? ? 48 89", [](soup::Pointer p)
		{
			pointers::connection_mgr = p.add(2).rip().as<rage::netConnectionManager**>();
		});
		BATCH_ADD_MANDATORY_HOOK("Q4", CNetworkSession_OnSessionEvent, "48 8B C4 48 89", "58 08 48 89 70 10 48 89 78 18 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 49 8B F8");
		BATCH_ADD_OPTIONAL("Q5", "74 52 44 39 67 0C 7F 4C", [](soup::Pointer p)
		{
			pointers::netIceSession_terminationTimer_IsTimedOut = p.as<uint8_t*>();
		});
		BATCH_ADD_FUNC("Q6", rage_datBitBuffer_WriteUnsigned, "48 89 5C 24 08", "44 8B DA 83 CB FF 4D 63 D1 49 C1 FA 03");
		BATCH_ADD("Q7", "45 33 F6 4C 8B F9 48 8D", [](soup::Pointer p)
		{
			p = p.sub(0x00007FF7DB406578 - 0x00007FF7DB40654C);
			CHECK_EXISTING_HOOK("Q7", "48 8B C4 48 89");
			STORE_HOOK(CNetworkSession_OnHandleJoinRequest);
		});
		BATCH_ADD("Q8", "80 78 1E 00 74 18 83 64 24 20 00 4C 8D 4C 24 30 44 8B C7 48 8B D0 48 8B CB E8", [](soup::Pointer p)
		{
			p = p.add(26).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("Q8", "48 89 5C 24 08");
			STORE_POINTER(send_netComplaintMsg); // rage::netConnectionManager::SendOutOfBand<rage::netComplaintMsg>
		});
		BATCH_ADD_OPTIONAL_FUNC("Q9", rage_netConnectionManager_SendConnectionless, "48 8B C4 48 89", "58 08 48 89 70 10 48 89 78 18 4C 89 48 20 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 44 8B BD");
		BATCH_ADD("QA", "4C 8D 85 40 06 00 00 48 8B CE 48 8B D0 E8", [](soup::Pointer p)
		{
			p = p.add(14).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QA", "48 85 D2 0F 84");
			STORE_POINTER(rage_snSession_HandleRemoveGamerCmd);
		});
		BATCH_ADD_FUNC("QB", CBlacklistedGamers_BlacklistGamer, "48 8B C4 48 89", "58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 40 45 8B F1"); // caller: 83 FE 04 74 ? 85 F6
		/*BATCH_ADD("QC", "48 8B 05 ? ? ? ? 48 89 3D ? ? ? ? 39 B8 B0", [](soup::Pointer p)
		{
			pointers::network_session = p.add(3).rip().as<CNetworkSession**>();
		});*/
#if I_CAN_SPY
		BATCH_ADD_FUNC("QC", rage_netConnection_QueueFrame, "4C 8B DC 49 89", "5B 08 49 89 6B 10 49 89 73 18 57 41 54 41 55 41 56 41 57 48 83 EC 60 33 F6 F6 81");
#endif
		BATCH_ADD("QD", "48 8B 0D ? ? ? ? E8 ? ? ? ? 33 DB 84 C0 74 41", [](soup::Pointer p)
		{
			pointers::network_clock = p.add(3).rip().as<rage::netTimeSync**>();
		});
#if I_CAN_SPY
		BATCH_ADD("QE", "75 1A 45 8B C6 48 8B D5 48 8B CE E8", [](soup::Pointer p)
		{
			p = p.add(12).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QE", "48 89 5C 24 08");
			STORE_POINTER(rage_netConnectionManager_CreateConnection);
		});
		BATCH_ADD_FUNC("QF", CNetworkSession_BuildJoinRequest, "48 8B C4 4C 89", "48 20 44 89 40 18 48 89 50 10 48 89 48 08 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 33 F6"); // They obfuscated the caller (E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 83 4C 24 68 FF) in b3351
		BATCH_ADD("QG", "48 8B D3 48 8B CD E8 ? ? ? ? 40 B7 01 49 8B CF", [](soup::Pointer p)
		{
			p = p.add(7).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QG", "48 89 5C 24 10");
			STORE_POINTER(rage_netConnectionManager_DestroyConnection);
		});
#endif
		// QH is used by ComponentSpoofPos
		BATCH_ADD("QI", "74 0B 41 BC 18 00 00 00", [](soup::Pointer p)
		{
			pointers::custombjmsg_nop2bytes = p.as<uint8_t*>();
			pointers::custombjmsg_responsecode = p.add(4).as<int32_t*>();
		});
		BATCH_ADD("QJ", "E8 ? ? ? ? 44 8B 86 80 00 00 00 48 8B 96 88 00 00 00 48 8D 8D F0 00 00 00 45 33 C9 E8", [](soup::Pointer p)
		{
			// now within rage::netIceTunneler::OnCxnEvent
			auto og = p;

			p = og.add(1).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QJ", "48 89 5C 24 08");
			STORE_POINTER(rage_netIceSessionOffer_ctor);

			p = og.add(30).rip();
			// now within rage::netIceSessionOffer::Import
			p = p.add((0x00007FF7C1A15AD9 - 0x00007FF7C1A15A70) + 1).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QI", "48 89 5C 24 08");
			STORE_POINTER(rage_netIceSessionOffer_SerMsgPayload_datImportBuffer);
		});
		BATCH_ADD("QK", "45 33 C9 E8 ? ? ? ? 84 C0 0F 84 48 01 00 00", [](soup::Pointer p)
		{
			// now within rage::netIceTunneler::OnCxnEvent
			p = p.add(4).rip();
			// now within rage::netIceSessionAnswer::Import
			p = p.add((0x00007FF7C1A15A1D - 0x00007FF7C1A159B4) + 1).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QK", "48 89 5C 24 08");
			STORE_POINTER(rage_netIceSessionAnswer_SerMsgPayload_datImportBuffer);
		});
		BATCH_ADD("QL", "41 80 BC 08 30 01 00 00 FF 75 5F 48 8B 05", [](soup::Pointer p)
		{
			pointers::net_array_mgr = p.add(14).rip().as<rage::netArrayManager**>();
		});
		BATCH_ADD("QM", "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 45 85 ED", [](soup::Pointer p)
		{
			pointers::rage_netGameEvent_ms_pPool = p.add(3).rip().as<rage::fwBasePool**>();
		});
#if I_CAN_SPY
		/*BATCH_ADD("QO", "41 B8 16 00 00 00 E8 ? ? ? ? E8", [](soup::Pointer p)
		{
			p = p.add(12).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QO", "40 53 48 83 EC");
			STORE_POINTER(rage_netRelay_GetMyRelayAddress);
		});*/
		BATCH_ADD("QO", "49 8D BF B4 02 00 00 E8 ? ? ? ? 49 8B 8F A0 00 00 00", [](soup::Pointer p)
		{
			auto og = p;

			p = p.add(8).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QO", "40 53 48 83 EC");
			STORE_POINTER(rage_netRelay_GetMyRelayAddress);

			p = og.add(0x00007FF725548DEA - 0x00007FF725548D51).add(1).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("QO", "48 8B C4 48 89");
			STORE_POINTER(rage_snMsgJoinRequest_Export);
		});
		BATCH_ADD("QP", "48 8D BB 1C 01 00 00 41 B8 0A 00 00 00", [](soup::Pointer p)
		{
			p = p.sub(0x00007FF7254ABB92 - 0x00007FF7254ABB24);
			CHECK_EXISTING_HOOK_WONT_HOOK("QP", "48 89 5C 24 08");
			STORE_POINTER(rage_snMsgAddGamerToSessionBase_SerMsgPayload_datImportBuffer);
		});
#if SPY_CAN_KICK
		BATCH_ADD_FUNC("QQ", rage_netConnectionManager_OpenTunnel, "48 89 5C 24 08", "4C 89 4C 24 20 48 89 54 24 10 55 56 57 41 54 41 55 41 56 41 57 48 83 EC 60 4D 8D 60 08");
#endif
#endif
		BATCH_ADD_OPTIONAL_FUNC("QR", rage_netConnectionManager_Send, "48 8B C4 48 89", "58 08 48 89 68 10 48 89 70 18 44 89 48 20 57 41 54 41 55 41 56 41 57 48 83 EC 30 4C 8B F9");
		BATCH_ADD_OPTIONAL_HOOK("QS", rage_netIceSession_GetAdditionalLocalCandidates, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 20 57 41 54 41 55 41 56 41 57 48 83 EC 20 45 33 ED");

		BATCH_ADD_OPTIONAL_HOOK("R0", rlGamerHandle_from_friend_index, "40 53 48 83 EC", "20 8B D9 83 F9 FE 75 1C E8");
		BATCH_ADD("R1", "48 8D 34 80 48 C1 E6 04 48 03 35 ? ? ? ? 48 8B CE E8", [](soup::Pointer p)
		{
			pointers::menu_ptr = p.add(11).rip().as<uint64_t*>();
		});
		BATCH_ADD("R2", "48 89 74 24 20 55 57 41 54 41 56 41 57 48 8D AC 24 10 FF FF FF", [](soup::Pointer p)
		{
			p = p.sub(5);
			CHECK_EXISTING_HOOK_WONT_HOOK("R2", "48 89 5C 24 10");
			pointers::CPlayerListMenu_HandleContextOption = p.as<CPlayerListMenu_HandleContextOption_t>();
			pointers::CPlayerListMenu_handler_jnz = p.add(0x00007FF7C0A5C424 - 0x00007FF7C0A5C3E0).as<uint8_t*>();
			pointers::CPlayerListMenu_handler_jz = p.add(0x00007FF7C0A5C450 - 0x00007FF7C0A5C3E0).as<uint8_t*>();
			pointers::rlGamerHandle_buffer = p.add(0x00007FF7C0A5C452 - 0x00007FF7C0A5C3E0).add(3).rip().as<rage::rlGamerHandle*>();
		});
		/*BATCH_ADD("R3", "48 8B 0D ? ? ? ? 48 8D 3D ? ? ? ? F3 0F 7F 40 D8 8B 51 20 E8", [](soup::Pointer p)
		{
			pointers::get_card_mgr_args = p.add(3).rip().as<uint64_t*>();
			pointers::get_card_mgr = p.add(23).rip().as<get_card_mgr_t>();
		});*/
		BATCH_ADD("R3", "48 8B 0D ? ? ? ? 33 D2 E8 ? ? ? ? 45 33 C0 48 8D 4C 24 30 41 8D 70 04 89 44 24 30 8B D6 E8 ? ? ? ? 8B D8 E8", [](soup::Pointer p)
		{
			auto og = p;

			pointers::extra_content_manager = p.add(3).rip().as<CExtraContentManager**>();

			p = og.add(10).rip();
			CHECK_EXISTING_HOOK("R3", "48 89 5C 24 08");
			STORE_HOOK(CExtraContentManager_GetCRC);

			p = og.add(40).rip();
			CHECK_EXISTING_HOOK("R4", "48 89 5C 24 10");
			STORE_HOOK(get_matchmaking_pool);

			p = og.add(95);
			CHECK_EXISTING_HOOK("R5", "48 8B 0D");
			p = p.add(3).rip();
			STORE_POINTER(asset_verifier);
		});
		BATCH_ADD("R6", "49 8D 57 10 48 8D 4C 24 20 40 8A FD E8", [](soup::Pointer p)
		{
			pointers::rage_datImport_netPeerAddress = p.add(13).rip().as<rage_datImport_netPeerAddress_t>();
		});
		BATCH_ADD("R7", "48 63 B9 ? ? 00 00 48 8D 05", [](soup::Pointer p)
		{
			pointers::ros_credentials = p.add(10).rip().as<rage::rlRosCredentials*>();
		});
		BATCH_ADD("R8", "4C 8D 4D 88 4C 8D 44 24 60 48 8D 15", [](soup::Pointer p)
		{
			pointers::rlPresence_GamerPresences_0_GamerInfo = p.add(12).rip().as<rage::rlGamerInfo*>();
			// Could also get rage::rlPresence::m_Delegator and rage::atDelegator::Dispatch here
		});
		BATCH_ADD("R9", "80 78 ? 00 75 10 48 8D 0D ? ? ? ? E8", [](soup::Pointer p)
		{
			pointers::g_rlPc = p.add(9).rip().as<rage::rlPc*>();
		});
		{
			SIG_INST("8B 05 ? ? ? ? 4C 8D 15 ? ? ? ? 45 33 C0");
			batch.add_optional("RA", soup::Module("socialclub.dll"), sig_inst, [](soup::Pointer p)
			{
				pointers::num_presence_attributes = p.add(2).rip().as<uint32_t*>();
				pointers::presence_attributes = p.add(2).rip().add(12).as<rage::rlScPresenceAttribute*>();
			});
		}
		BATCH_ADD_FUNC("RB", rage_netPeerAddress_Export, "48 89 5C 24 10", "48 89 74 24 18 48 89 7C 24 20 55 41 56 41 57 48 8B EC 48 83 EC 50 48 8B F1");
		// RC pattern would need to be updated for b3351
		/*BATCH_ADD("RC", "4C 8D 83 ? ? ? ? E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? C7 43 ? ? ? ? ? B8 ? ? ? ? E9", [](soup::Pointer p)
		{
			p = p.add(8).rip();
			CHECK_EXISTING_HOOK("RD", "48 8B C4 48 89");
			pointers::rage_rlScMatchmaking_Find = p.as<rage_rlScMatchmaking_Find_t>();
		});
		BATCH_ADD_OPTIONAL_HOOK("RD", rage_rlScMatchmakingFindTask_ReadSession, "40 53 48 83 EC", "20 48 8B CA 49 8B D8");
		BATCH_ADD_OPTIONAL_HOOK("RE", rage_rlScMatchmakingFindTask_ProcessSuccess, "48 89 5C 24 08", "48 89 74 24 10 57 48 81 EC F0 00 00 00 41 83 21 00");*/
		BATCH_ADD("RF", "48 89 44 ? ? E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 4C ? ? E8 ? ? ? ? 48 8D 54", [](soup::Pointer p)
		{
			pointers::rlRos_s_GeoLocInfo = p.add(13).rip().as<rage::rlRosGeoLocInfo*>();
		});
		BATCH_ADD("RG", "48 8D 0D ? ? ? ? 4C 8B CF 48 8B D6 4C 8B C0 44 89 64 24 20 E8", [](soup::Pointer p)
		{
			pointers::invite_mgr = p.add(3).rip().as<InviteMgr*>();
			pointers::InviteMgr_HandleJoinRequest = p.add(22).rip().as<InviteMgr_HandleJoinRequest_t>();
		});
		BATCH_ADD_FUNC("RH", rage_rlSessionManager_QueryDetail, "48 8B C4 44 89", "48 20 44 89 40 18 89 50 10 89 48 08");
		BATCH_ADD_FUNC("RI", rage_rlScPresence_GetAttributesForGamer, "48 8B C4 48 89", "58 08 48 89 68 10 48 89 70 18 4C 89 70 20 41 57 48 83 EC 40 4C 8B F2");
		/*BATCH_ADD("RJ", "E8 ? ? ? ? 0F B6 CB 84 C0 41 0F 44 CD", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			STORE_HOOK(CNetworkSession_JoinSession);
		});*/
		BATCH_ADD_OPTIONAL("RK", "E8 ? ? ? ? 84 C0 74 04 B0 01 EB 02 32 C0 48 8B 5C 24 ? 48 8B 74 24 ? 48 8B 7C 24 ? 48 83 C4 20", [](soup::Pointer p)
		{
			p = p.add(1).rip();
			CHECK_EXISTING_HOOK("RK", "48 89 5C 24 08");
			STORE_HOOK(rage_playerDataMsg_SerMsgPayload_datExportBuffer);
		});
		BATCH_ADD_OPTIONAL("RL", "48 83 EC 40 48 8B 05 ? ? ? ? 66 44 89 4C 24", [](soup::Pointer p)
		{
			p = p.sub(0x15);
			CHECK_EXISTING_HOOK("RL", "48 89 5C 24 18");
			STORE_HOOK(rage_netArrayHandlerBase_WriteUpdate);
		});
		BATCH_ADD("RM", "74 73 FF 90 ? ? ? ? 8B D5 4C 8B 00 48 8B C8 41 FF 50 30", [](soup::Pointer p)
		{
			pointers::rage_netArrayHandlerBase_WriteUpdate_patch = p.as<uint8_t*>();
		});
		/*BATCH_ADD("RN", "8A 84 24 30 01 00 00 48 89 15 ? ? ? ? 8B 94 24 20 01 00 00", [](soup::Pointer p)
		{
			pointers::session_id = p.add(10).rip().as<uint64_t*>();
		});*/
		BATCH_ADD("RN", "B9 E0 07 00 00 E8", [](soup::Pointer p)
		{
			pointers::send_session_info_request = p.sub(0x00007FF71EAA2F19 - 0x00007FF71EAA2EBC).as<send_session_info_request_t>();
		});
		BATCH_ADD("RO", "48 8D A8 F8 FE FF FF 48 81 EC F0 01 00 00 33 F6", [](soup::Pointer p)
		{
			p = p.sub(20);
			CHECK_EXISTING_HOOK("RO", "48 8B C4 48 89");
			STORE_HOOK(rage_rlGetGamerStateTask_ParseResults);
		});
		{
			SIG_INST("40 53 48 83 EC 40 48 8B 01 48 8B D9");
			batch.add_optional("RP", soup::Module("socialclub.dll"), sig_inst, [](soup::Pointer p)
			{
				STORE_POINTER(rgsc_Script_CallFunction);
			});
		}
		{
			SIG_INST("48 89 45 C8 48 8B CE E8");
			batch.add_optional("RQ", soup::Module("socialclub.dll"), sig_inst, [](soup::Pointer p)
			{
				p = p.add(8).rip();
#ifdef STAND_DEBUG
				CHECK_EXISTING_HOOK("RQ", "48 89 5C 24 08");
#endif
				STORE_HOOK(rgsc_RgscPresenceManager_OnSocialClubEvent);
			});
		}

		BATCH_ADD("S0", "0F 84 E1 00 00 00 48 8D 57 08 48 8B CB E8 ? ? ? ? 84 C0 0F 84 CD 00 00 00", [](soup::Pointer p)
		{
			p = p.add(14).rip();
			CHECK_EXISTING_HOOK("S0", "48 89 5C 24 08");
			STORE_HOOK(send_net_info_to_lobby_wrap);
		});
		BATCH_ADD_OPTIONAL("S3", "48 8B 81 A8 00 00 00 48 83 C0 20 C3", [](soup::Pointer p)
		{
			STORE_HOOK(CNetGamePlayer_GetGamerInfo);
		});
		BATCH_ADD("S4", "84 C0 74 23 8B CB E8 ? ? ? ? 48 83 38 00", [](soup::Pointer p)
		{
			p = p.add(7).rip();
			CHECK_EXISTING_HOOK("S4", "85 C9 75 19 48");
			pointers::rage_rlClan_GetPrimaryMembership = p.as<rage_rlClan_GetPrimaryMembership_t>();
			pointers::clan_client = p.add(10).rip().as<rage::rlClanClient*>();
		});
		/*BATCH_ADD("S5", "8A 4B 33 E8 ? ? ? ? EB 23 48 8D 4B 3C C6 44 24 30 01", [](soup::Pointer p)
		{
			auto og = p;

			p = og.add((0x00007FF7FC524E5A - 0x00007FF7FC524E32) + 1).rip();
			CHECK_EXISTING_HOOK("S5", "48 89 5C 24 08");
			STORE_HOOK(on_crc_hash_check_start);

			p = og.add((0x00007FF7FC524E35 - 0x00007FF7FC524E32) + 1).rip();
			CHECK_EXISTING_HOOK("S6", "48 89 5C 24 08");
			STORE_HOOK(on_crc_hash_check_reply);
		});*/
		BATCH_ADD_OPTIONAL("S7", "49 8B D6 48 8B CE E8 ? ? ? ? 0F 2E 07 74 03", [](soup::Pointer p)
		{
			p = p.add(7).rip();
			CHECK_EXISTING_HOOK("S6", "48 8B C4 48 89");
			STORE_HOOK(get_player_card_stat);
		});
		BATCH_ADD_OPTIONAL_HOOK("S8", CPlayerGamerDataNode_Serialise, "48 89 5C 24 10", "48 89 6C 24 18 56 57 41 56 48 83 EC 20 48 8D 99 C0 00 00 00");
		BATCH_ADD_OPTIONAL("S9", "C6 44 24 20 01 41 FF 92 C0 00 00 00 8B 5E 3C", [](soup::Pointer p) // optional for when loading Stand on top of Stand
		{
			STORE_HOOK(data_node_write);
			data_node_helper_init(&hooks::data_node_write, p.add(12).as<void*>());
		});
		BATCH_ADD_OPTIONAL("SA", "48 8B FA 4C 8D 4D 50 4C 8D 45", [](soup::Pointer p)
		{
			p = p.sub(0x32);
			CHECK_EXISTING_HOOK("SA", "48 89 4C 24 08");
			STORE_HOOK(CNetObjVehicle_GetVehicleMigrationData);
		});
		// S1 & S2 are unused
		BATCH_ADD_OPTIONAL("SY", "48 8B 05 ? ? ? ? 8B CB 48 69 C9 E0 01 00 00 48 63 94 01 B0 00 00 00 48 8B 05", [](soup::Pointer p)
		{
			pointers::scaleform_array = p.add(3).rip().as<rage::atArray<ScaleformMovieStruct>*>();
			pointers::scaleform_store = p.add(27).rip().sub(offsetof(CScaleformStore, m_pool.m_aFlags)).as<CScaleformStore*>();
		});
		BATCH_ADD_OPTIONAL("SZ", "0F 94 C0 88 44 24 20 E8 ? ? ? ? 48 8B 05", [](soup::Pointer p)
		{
			// ped_factory is also here if we needed it
			p = p.add(8).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("SZ", "48 89 5C 24 08");
			STORE_POINTER(GFxValue_ObjectInterface_GetMember);
		});

		BATCH_ADD("V0", "48 83 25 ? ? ? ? 00 48 8D 0D ? ? ? ? E8 ? ? ? ? E8", [](soup::Pointer p)
		{
			p = p.add(16).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("V0", "48 83 EC 28 48");
			STORE_POINTER(CVisualSettings_LoadAll);
		});
		BATCH_ADD_OPTIONAL("V1", "41 B1 01 48 8B CB 0F 28 00 0F 29 05 ? ? ? ? E8", [](soup::Pointer p)
		{
			auto og = p;

			p = og.sub(19).rip();
			CHECK_EXISTING_HOOK("V1", "48 8B C4 48 89");
			STORE_HOOK(get_visual_config_colour);

			p = og.add(17).rip();
			CHECK_EXISTING_HOOK("V2", "48 89 5C 24 08");
			STORE_HOOK(get_visual_config_float);
		});
		BATCH_ADD_OPTIONAL("V3", "41 0F 28 D9 48 8B CB E8 ? ? ? ? 48 8D 15", [](soup::Pointer p)
		{
			p = p.add(8).rip();
			CHECK_EXISTING_HOOK("V3", "40 53 48 83 EC");
			STORE_HOOK(get_visual_config_float_with_directory);
		});
		BATCH_ADD_OPTIONAL("V4", "74 ? 48 8D 15 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 8B 0E", [](soup::Pointer p)
		{
			p = p.add(13).rip();
			CHECK_EXISTING_HOOK("V4", "48 89 5C 24 08");
			STORE_HOOK(camFactory_CreateObject);
		});
		// V5 & V6 are used by ComponentGpu
		// V7, V8, V9, & VA are used by ComponentDrawPatch
		BATCH_ADD_MANDATORY_HOOK("VB", camFrame_copy, "48 89 5C 24 08", "57 48 83 EC 20 8B 42 40 F3 0F 10 4A 48");

		BATCH_ADD_OPTIONAL_HOOK("X0", some_player_sync, "48 89 5C 24 08", "55 56 57 41 54 41 55 41 56 41 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8B FA 48 8B F1");
		BATCH_ADD_OPTIONAL("X1", "0F 28 D6 48 8B D3 48 8B CF E8 ? ? ? ? 8B F0 EB 4F", [](soup::Pointer p)
		{
			p = p.add(10).rip();
			CHECK_EXISTING_HOOK("X1", "48 89 5C 24 10");
			STORE_HOOK(rage_aiTaskTree_UpdateTask);
		});
		BATCH_ADD_OPTIONAL("X2", "48 8B 48 20 48 85 C9 0F 84 E3 00 00 00 E8", [](soup::Pointer p)
		{
			p = p.add(14).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("X2", "33 C0 F6 41 50");
			STORE_POINTER(rage_fwArchetype_GetFragType);
		});
		// The rest is in ComponentCrashPatch

#ifdef STAND_DEBUG
		BATCH_ADD_OPTIONAL_HOOK("Z0", rage_netMessage_WriteHeader, "48 89 5C 24 08", "48 89 74 24 10 57 48 83 EC 20 48 8B DA BE 01 00 00 00 32 D2 8B F9 40");
#endif
#ifdef STAND_DEV
		BATCH_ADD_OPTIONAL_HOOK("Z1", rage_netSyncDataNode_Read, "48 89 5C 24 08", "48 89 74 24 18 57 48 83 EC 20 40 8A 79 40 49 8B F1");
#endif

		BATCH_ADD("ZC", "C3", [](soup::Pointer p)
		{
			STORE_POINTER(nullsub);
		});

		Components::addPatterns(batch);

		// Scan
		auto scan_patterns = batch.entries.size();
		auto scan_time = TimedCall::run([&]
		{
			batch.run();
		});

		// Determine game version
		std::string game_version{};
		if (pointers::online_version != nullptr)
		{
			game_version = pointers::online_version;
			if (pointers::build_version != nullptr)
			{
				game_version.push_back('-');
				game_version.append(pointers::build_version);
			}
		}
		else
		{
			if (pointers::build_version != nullptr)
			{
				game_version = pointers::build_version;
			}
		}
		
		// Failed to scan any patterns?
		if (!batch.error_message.empty())
		{
			if (!game_version.empty())
			{
				std::string stand_game_version = soup::ObfusString(GTA_VERSION_TARGET).str();
				if (game_version != stand_game_version)
				{
					batch.error_message.append(soup::ObfusString(" (Note: Found game version ").str());
					batch.error_message.append(game_version);
					batch.error_message.append(soup::ObfusString(", but this Stand version is for game version ").str());
					batch.error_message.append(stand_game_version);
					batch.error_message.push_back(')');
				}
			}
			showMessageBoxAndLog(batch.error_message.c_str());
			return false;
		}

		// Print post-scan info
		{
			std::string message = soup::ObfusString("Scanned ");
			if (window_title == soup::ObfusString("Grand Theft Auto V").str())
			{
				message.append(soup::ObfusString("Stealy V-eely Automobiley").str());
			}
			else
			{
				ColoadMgr::coloading_with_any_menu = true;
				if (window_title == soup::ObfusString("PopstarV").str())
				{
					ColoadMgr::coloading_with_2take1menu = true;
				}
				if (window_title.find('\0') != std::string::npos)
				{
					message.append(soup::ObfusString("Grand Theft Auto V").str());
				}
				else
				{
					message.append(window_title);
				}
			}
			if (!game_version.empty())
			{
				message.push_back(' ');
				message.append(game_version);
			}
			message.append(soup::ObfusString(" for ").str()).append(fmt::to_string(scan_patterns)).append(soup::ObfusString(" sigs in ").str()).append(scan_time);
			if (batch.cache_utilisation != 0)
			{
				message.append(fmt::format(fmt::runtime(soup::ObfusString(" ({} from cache)").str()), batch.cache_utilisation));
			}
			if (pointers::is_explosion_type_valid_patch == nullptr)
			{
				message.append(soup::ObfusString(" and it looks like you may have a corrupt cache. Refusing to load.").str());
				g_logger.log(std::move(message));
				PointerCache::saveAndDeinit();
				return false;
			}
			if (*pointers::is_explosion_type_valid_patch == CS_ACTIVE)
			{
				message.append(soup::ObfusString(" and found that Stand is already injected. Refusing to load.").str());
				g_logger.log(std::move(message));
				PointerCache::saveAndDeinit();
				return false;
			}
			if (!existing_hooks.empty() || !batch.allowed_fails.empty())
			{
				bool failed_due_to_coload = (!existing_hooks.empty()
					&& existing_hooks != soup::ObfusString("G3").str() // FSL
					);

#ifdef STAND_DEV
				message.append(soup::ObfusString(" with ").str());
				if (!existing_hooks.empty())
				{
					message.append(soup::ObfusString("existing hooks (").str()).append(existing_hooks).push_back(')');
					existing_hooks.clear();
					if (!batch.allowed_fails.empty())
					{
						message.append(soup::ObfusString(" and ").str());
					}
				}
				if (!batch.allowed_fails.empty())
				{
					message.append(soup::ObfusString("some restrictions (").str()).append(batch.allowed_fails).push_back(')');
				}
#endif

				if (!ColoadMgr::coloading_with_any_menu)
				{
					if (!failed_due_to_coload)
					{
						auto fails_vec = soup::string::explode(batch.allowed_fails, ", ");
						for (const auto& pattern_name : fails_vec)
						{
							if (pattern_name != Codename("D2").toString()
								&& pattern_name != Codename("D3").toString()
								&& pattern_name != Codename("RA").toString()
								&& pattern_name != Codename("RP").toString()
								&& pattern_name != Codename("RQ").toString()
								)
							{
								failed_due_to_coload = true;
								break;
							}
						}
					}
					if (failed_due_to_coload)
					{
						ColoadMgr::coloading_with_any_menu = true;

						// existing hooks (C9, HJ, L0, O1, O2, O3, O5, P7, PJ, QO, R3, RE, RK, X3, X7, X9, XM, VA, Q0) and some restrictions (HC)
						/*if (existing_hooks.find(soup::ObfusString("HJ").str()) != std::string::npos)
						{
							ColoadMgr::coloading_with_cherax = true;
						}*/
					}
				}
			}
			g_logger.log(std::move(message));
		}

#if ENABLE_PASSIVE_DLL
		if (*pointers::is_explosion_type_valid_patch == CS_PASSIVE)
		{
			g_game_has_passive_dll = true;
		}
#endif
		*pointers::is_explosion_type_valid_patch = CS_ACTIVE;
		patched_is_explosion_type_valid = true;

		return true;
	}

	void mainUnpatch()
	{
		if (pointers::radio_talk02_hash
			&& *pointers::radio_talk02_hash != ATSTRINGHASH("RADIO_11_TALK_02")
			)
		{
			g_logger.log(soup::ObfusString("Blaine County Radio patch still in effect?! Cleaning that up now.").str());
			*pointers::radio_talk02_hash = ATSTRINGHASH("RADIO_11_TALK_02");
		}

		pointers::self_radio_patch.restore();
		pointers::watchdog_thread_crash_patch.restore();
		pointers::model_blacklist_patch.restore();
		pointers::point_gun_at_head_patch.restore();
#if PREVENT_PGSTREAMER_CRASH
		pointers::pgstreamer_crash_patch.restore();
#endif
#if PREVENT_GBUFFER_CRASH
		pointers::gbuffer_crash_patch.restore();
#endif
#if PREVENT_GRCDEVICE_CRASH
		pointers::grcdevice_crash_patch.restore();
#endif
	}

	void mainStopCountedThreads()
	{
		uint8_t counter = 0;
		while (Exceptional::counted_threads != 0)
		{
			soup::os::sleep(10);
			if (IS_TAKING_TOO_LONG(50))
			{
				g_logger.log(soup::ObfusString("#4 seems to be holding us up."));
#ifdef STAND_DEBUG
				for (const auto& name : Exceptional::counted_thread_names)
				{
					g_logger.log(fmt::format("- {}", name));
				}
#endif
			}
		}
	}

	static uint8_t running_loop_calls = 0;

	struct RunningLoopState
	{
		enum Stage : uint8_t
		{
			INITIAL = 0,
			LOOPING,
			FINISHED,
		};

		Stage stage = INITIAL;
		uint8_t time_fallback_unload = 0;
		uint8_t keypress_fallback_unload = 0;
		uint8_t bad_state_unload = 0;
		uint8_t perm_expiry = 0;
	};

#define finishRunningLoop st.stage = RunningLoopState::FINISHED; return false;

	static std::string clipboard_backup{};

	[[nodiscard]] static __declspec(noinline) bool mainRunningLoopIteration(RunningLoopState& st) // OBFUS!
	{
		if (st.stage == RunningLoopState::INITIAL)
		{
			st.stage = RunningLoopState::LOOPING;

			++running_loop_calls;
			if (running_loop_calls > (did_minimal_init ? 2 : 1))
			{
				finishRunningLoop;
			}
		}
		else if (st.stage == RunningLoopState::FINISHED)
		{
			return false;
		}

		ensureAntiDump();

		// Check overdue unload
		if (g_gui.isUnloadPending())
		{
			if (++st.time_fallback_unload == 150) // 30 seconds
			{
				g_logger.log(soup::ObfusString("It looks like an unload was scheduled but not realised. So, here we go."));
				finishRunningLoop;
			}
		}
		else
		{
			st.time_fallback_unload = 0;
		}
		// Keypress fallback unload
		if (is_special_key_being_pressed())
		{
			if (g_gui.root_state == GUI_MINIMAL)
			{
				finishRunningLoop;
			}
			if (++st.keypress_fallback_unload == 7) // * 200ms = 1400ms
			{
				g_logger.log(soup::ObfusString("F1 or F9 is being held down"));
				finishRunningLoop;
			}
		}
		else
		{
			st.keypress_fallback_unload = 0;
		}
		// Manage PointerCache + killswitch
		SOUP_IF_LIKELY (g_tunables.ready)
		{
			SOUP_IF_UNLIKELY (PointerCache::isInited())
			{
				if (PointerCache::cache.contains(soup::ObfusString(STAND_VERSION).str()))
				{
					g_gui.killswitched = true;
				}
				if (g_gui.initial_auth_done)
				{
					// Pop suspension even if auth was unsuccessful to avoid this being too aggressive.
					if (auto e = PointerCache::cache.find(Codename("AZ").toString()); e != PointerCache::cache.end())
					{
						PointerCache::cache.erase(e);
					}

					PointerCache::saveAndDeinit();
				}
			}
		}
		// Check bad state
		{
			uint8_t bad_state = 0;
			if (Sanity::playerhistory_find_in_non_normal_root_state)
			{
				bad_state = 20;
			}
			else if (Sanity::playerhistory_insert_in_auth_inconsistent_state)
			{
				bad_state = 21;
			}
			else if (Sanity::discovered_other_player_without_api_credentials)
			{
				bad_state = 22;
			}
			else if (Sanity::discovered_other_player_in_invalid_root_state)
			{
				bad_state = 23;
			}
			else if (Sanity::discovered_other_player_non_free_without_recover_state)
			{
				bad_state = 24;
			}
			else if (auto dbgid = AntiDbg::getRunningDebuggerId(false))
			{
				if (dbgid >= 100)
				{
					AntiDbg::apologiseInAdvance(4);
					AntiDbg::enqueueSuspension(dbgid);
					AntiDbg::bToTheSToTheOToTheD();
					dbgid -= 100;
				}
				bad_state = dbgid + 100;
			}
			else if (!CodeIntegrity::verify())
			{
				bad_state = 25;
			}
			else if (g_gui.root_state == GUI_MINIMAL)
			{
				if (!did_minimal_init)
				{
					bad_state = 10;
				}
				else if (has_game_finished_loading())
				{
					if (running_loop_calls == 1)
					{
						finishRunningLoop;
					}
					else if (running_loop_calls != 2)
					{
						bad_state = 2;
					}
				}
			}
			if (bad_state == 0)
			{
				if (st.perm_expiry != 0)
				{
					bad_state = 6;
				}
				else if (g_renderer.doesGameplayStateAllowScriptExecution())
				{
					if (g_gui.about_to_update_root_state)
					{
						bad_state = 3;
					}
					else if (is_session_started() != (bool)NETWORK::NETWORK_IS_GAME_IN_PROGRESS()) // This native uses the same value. Is ours being tampered with?
					{
						bad_state = 4;
					}
					else if (g_gui.root_state == GUI_FREE
						&& (AbstractPlayer::is_freemode || NETWORK::NETWORK_IS_SESSION_STARTED())
						)
					{
						bad_state = 5;
					}
				}
			}
			if (bad_state == 0)
			{
				if (g_auth.license_permissions != LICPERM_FREE && g_auth.activation_key_to_try.empty())
				{
					bad_state = 11;
				}
				else if (!g_gui.isRootStateFull()
					&& g_gui.root_list->children.size() > 5
					)
				{
					bad_state = 7;
				}
				else if (g_gui.shouldRootStateShowLicensePrompt()
					&& g_gui.getActivateTab() == nullptr
					)
				{
					bad_state = 8;
				}
				/*else if (*pointers::session_id != 0
					&& *pointers::session_id != SessionSpoofing::getRealSessionId()
					)
				{
					//g_logger.log(fmt::format("{} =/= {}", *pointers::session_id, SessionSpoofing::getRealSessionId()));
					bad_state = 12;
				}*/
			}
			if (bad_state != 0)
			{
				// 1 & 9 are unused
				if (bad_state >= 10 || ++st.bad_state_unload == 15)
				{
					// This will brick future attempts to update the grid if a content update is not queued right now
					g_menu_grid.content_update_queued = true;

					g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("It looks like Stand is in a bad state ({})").str()), bad_state));
					//g_auth.reportEvent("D1", fmt::to_string(bad_state));

					finishRunningLoop;
				}
			}
			else
			{
				st.bad_state_unload = 0;
			}
		}
		// No longer running? Exit this damn loop.
		if (!g_running)
		{
			finishRunningLoop;
		}
		if (!g_gui.isUnloadPending())
		{
			// Log out if perm has expired (or is invalid)
			if (g_auth.license_permissions != LICPERM_FREE && g_auth.verifyPermSig() < g_auth.license_permissions)
			{
				if (++st.perm_expiry == 5)
				{
					g_auth.logOut(true);
					Util::toast(Label::combineWithSpace(LOC("AUTH_X"), LOC("CONHELP")), TOAST_ABOVE_MAP);
				}
			}
			else
			{
				if (st.perm_expiry != 0)
				{
					--st.perm_expiry;
				}
			}
			// Ensure we have the server data that we need
			if (!g_auth.activation_key_to_try.empty())
			{
				if (g_tunables.version == 0)
				{
					if (!g_tunables.downloading)
					{
						g_tunables.download();
					}
				}
				else if (!g_auth.sent_next_heartbeat
					&& (pointers::rlPresence_m_ActingGamerInfo->getHandle().rockstar_id != 0 || g_gui.root_state != GUI_MINIMAL)
					)
				{
					auto session = SessionSpoofing::getRealSessionId();
					if (!g_auth.perm_sig.empty())
					{
						const bool is_different_session = g_auth.isDifferentSession(session);
						if (get_seconds_since_unix_epoch() >= g_auth.next_heartbeat
							|| is_different_session
							)
						{
							g_auth.sendHeartbeat(session);
						}
						if (is_different_session)
						{
							g_auth.stand_user_identification_this_session = false;
						}

						// Really dirty, but since we don't have BE running, we need these fixed to be able to join others.
						/*(*pointers::asset_verifier)->m_CRC.Set(0x124ea49d);
						(*pointers::asset_verifier)->m_StaticCRC.Set(0xcd3ac8fd);
						(*pointers::asset_verifier)->m_BattlEye.Set(0);*/
					}
					else
					{
						if (get_seconds_since_unix_epoch() >= g_auth.next_heartbeat)
						{
							g_auth.sendHeartbeat(session);
						}
					}
				}
			}
			if (g_gui.root_state != GUI_MINIMAL)
			{
				// Check clipboard
				auto clipboard = Util::getClipboardTextUTF8();
				bool handled = false;
				if (clipboard.substr(0, 6) == soup::ObfusString("Stand-").str())
				{
					if (clipboard.substr(6, 9) == soup::ObfusString("Activate-").str())
					{
						soup::string::rtrim(clipboard);
						if (g_auth.activation_key_to_try.empty()
							&& clipboard.length() == 6 + 9 + ACTIVATION_KEY_CHARS
							)
						{
							handled = true; mainFlashWindow();
							g_auth.activation_key_to_try = clipboard.substr(6 + 9);
							if (g_auth.activation_key_to_try == g_auth.crash_key)
							{
								g_auth.next_heartbeat = LLONG_MAX;
							}
							else
							{
								g_auth.enableNotifications();
								g_auth.tryActivationKey();
								Util::toast(LOC("ACTVTE_STRT2"), TOAST_ABOVE_MAP);
							}
						}
					}
					else if (clipboard.substr(6, 5) == soup::ObfusString("Join-").str())
					{
						handled = true; mainFlashWindow();
						JoinUtil::connectViaCode(clipboard.substr(6 + 5), false);
					}
					else if (clipboard.substr(6, 9) == soup::ObfusString("Spectate-").str())
					{
						handled = true; mainFlashWindow();
						JoinUtil::connectViaCode(clipboard.substr(6 + 9), true);
					}
					else if (clipboard.substr(6, 6) == soup::ObfusString("Relay-").str())
					{
						handled = true;
						if (g_auth.license_permissions >= LICPERM_REGULAR)
						{
							g_relay.server = clipboard.substr(6 + 6);
							g_relay.init();
						}
					}
					else if (clipboard.substr(6, 11) == soup::ObfusString("Commandbox-").str())
					{
						handled = true; mainFlashWindow();
						g_gui.showCommandBox(StringUtils::utf8_to_utf16(clipboard.substr(6 + 11)));
					}
					else if (clipboard.substr(6, 6) == soup::ObfusString("Focus-").str())
					{
						handled = true; mainFlashWindow();
						if (auto cmd = g_gui.root_list->resolveCommandConfig(clipboard.substr(6 + 6)))
						{
							g_gui.open(TC_OTHER);
							cmd->focus(TC_OTHER);
						}
						else
						{
							std::string msg = soup::ObfusString("Command not found: ");
							msg.append(clipboard.substr(6 + 6));
							Util::toast(std::move(msg));
						}
					}
				}
				SOUP_IF_UNLIKELY (handled)
				{
					Util::copy_to_clipboard_utf8(clipboard_backup);
				}
				else
				{
					clipboard_backup = std::move(clipboard);
				}
			}
		}
		return true;
	}

	void mainRunningLoop()
	{
		RunningLoopState st{};
		while (mainRunningLoopIteration(st))
		{
			soup::os::sleep(200);
		}
	}

	void mainFlashWindow()
	{
		SetForegroundWindow(pointers::hwnd);
	}
}
