#pragma once

#include "asi_decl.hpp"

namespace Stand
{
	inline bool g_asi_main_script_registered = true;

	extern int createTexture(const char*);
	extern void drawTexture(int id, int index, int level, int time, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float screenHeightScaleFactor, float r, float g, float b, float a);
	extern void presentCallbackRegister(AsiPresentCallback cb);
	extern void presentCallbackUnregister(AsiPresentCallback cb);
	extern void keyboardHandlerRegister(KeyboardHandler handler);
	extern void keyboardHandlerUnregister(KeyboardHandler handler);
	extern void scriptWait(DWORD time);
	extern void scriptRegister(HMODULE module, void(*LP_SCRIPT_MAIN)());
	extern void scriptRegisterAdditionalThread(HMODULE module, void(*LP_SCRIPT_MAIN)());
	extern void scriptUnregister(HMODULE module);
	extern void scriptUnregister_old(void(*LP_SCRIPT_MAIN)());
	extern void nativeInit(UINT64 hash);
	extern void nativePush64(UINT64 val);
	extern PUINT64 nativeCall();
	extern UINT64* getGlobalPtr(int globalId);
	extern int worldGetAllVehicles(int* arr, int arrSize);
	extern int worldGetAllPeds(int* arr, int arrSize);
	extern int worldGetAllObjects(int* arr, int arrSize);
	extern int worldGetAllPickups(int* arr, int arrSize);
	extern BYTE* getScriptHandleBaseAddress(int handle);
	extern eGameVersion getGameVersion();

	using SSHV_init_t = void(*)(
		decltype(&createTexture),
		decltype(&drawTexture),
		decltype(&presentCallbackRegister),
		decltype(&presentCallbackUnregister),
		decltype(&keyboardHandlerRegister),
		decltype(&keyboardHandlerUnregister),
		decltype(&scriptWait),
		decltype(&scriptRegister),
		decltype(&scriptRegisterAdditionalThread),
		decltype(&scriptUnregister),
		decltype(&scriptUnregister_old),
		decltype(&nativeInit),
		decltype(&nativePush64),
		decltype(&nativeCall),
		decltype(&getGlobalPtr),
		decltype(&worldGetAllVehicles),
		decltype(&worldGetAllPeds),
		decltype(&worldGetAllObjects),
		decltype(&worldGetAllPickups),
		decltype(&getScriptHandleBaseAddress),
		decltype(&getGameVersion)
	);
}
