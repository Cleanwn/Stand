#pragma once

#include <string>

#include <soup/Module.hpp>
#include <soup/ModuleHider.hpp>

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID);

#ifdef STAND_DEBUG
#define HIDE_MODULE false
#else
#define HIDE_MODULE true
#endif

#define ENABLE_PASSIVE_DLL true

namespace Stand
{
	inline bool g_windows_7_or_older = false;
	inline bool g_wine = false;
	inline HMODULE g_hmodule{};
	inline LPTOP_LEVEL_EXCEPTION_FILTER g_og_unhandled_exception_filter{};
	inline bool g_running = false;
	inline std::string g_stand_dll_file_name{};
	inline soup::Range g_stand_dll_range{};
#if HIDE_MODULE
	inline soup::ModuleHider g_module_hider{};
#endif
	inline time_t g_last_successful_sig_check{};
	inline soup::Module g_gta_module = soup::Module(nullptr);
#if ENABLE_PASSIVE_DLL
	inline bool g_game_has_passive_dll = false;
	inline bool g_this_dll_is_passive = false;
#endif

	inline void enable_module_hider_if_applicable()
	{
#if HIDE_MODULE
		if (!g_windows_7_or_older && !g_wine)
		{
			g_module_hider.enable(g_hmodule);
		}
#endif
	}

	extern __declspec(noinline) BOOL onDllAttach(HMODULE hmod);
	extern __declspec(noinline) void mainInit();
	extern void mainInitDeferables();
	extern void mainThread();
	extern void mainDeinit();
	extern void mainUnload();
	extern void mainAfterPointers();
	extern bool mainInitPointers();
	extern void mainUnpatch();
	extern void mainStopCountedThreads();
	extern void mainRunningLoop();
	extern void mainFlashWindow();
}
