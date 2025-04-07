#include <asi_decl.hpp>

using namespace Stand;

using createTexture_t = int(*)(const char*);
using drawTexture_t = void(*)(int id, int index, int level, int time, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float screenHeightScaleFactor, float r, float g, float b, float a);
using presentCallbackRegister_t = void(*)(AsiPresentCallback cb);
using presentCallbackUnregister_t = void(*)(AsiPresentCallback cb);
using keyboardHandlerRegister_t = void(*)(KeyboardHandler handler);
using keyboardHandlerUnregister_t = void(*)(KeyboardHandler handler);
using scriptWait_t = void(*)(DWORD time);
using scriptRegister_t = void(*)(HMODULE module, void(*LP_SCRIPT_MAIN)());
using scriptRegisterAdditionalThread_t = void(*)(HMODULE module, void(*LP_SCRIPT_MAIN)());
using scriptUnregister_t = void(*)(HMODULE module);
using scriptUnregister_old_t = void(*)(void(*LP_SCRIPT_MAIN)());
using nativeInit_t = void(*)(UINT64 hash);
using nativePush64_t = void(*)(UINT64 val);
using nativeCall_t = PUINT64(*)();
using getGlobalPtr_t = UINT64*(*)(int globalId);
using worldGetAllVehicles_t = int(*)(int* arr, int arrSize);
using worldGetAllPeds_t = int(*)(int* arr, int arrSize);
using worldGetAllObjects_t = int(*)(int* arr, int arrSize);
using worldGetAllPickups_t = int(*)(int* arr, int arrSize);
using getScriptHandleBaseAddress_t = BYTE*(*)(int handle);
using getGameVersion_t = eGameVersion(*)();

static createTexture_t Stand_createTexture;
static drawTexture_t Stand_drawTexture;
static presentCallbackRegister_t Stand_presentCallbackRegister;
static presentCallbackUnregister_t Stand_presentCallbackUnregister;
static keyboardHandlerRegister_t Stand_keyboardHandlerRegister;
static keyboardHandlerUnregister_t Stand_keyboardHandlerUnregister;
static scriptWait_t Stand_scriptWait;
static scriptRegister_t Stand_scriptRegister;
static scriptRegisterAdditionalThread_t Stand_scriptRegisterAdditionalThread;
static scriptUnregister_t Stand_scriptUnregister;
static scriptUnregister_old_t Stand_scriptUnregister_old;
static nativeInit_t Stand_nativeInit;
static nativePush64_t Stand_nativePush64;
static nativeCall_t Stand_nativeCall;
static getGlobalPtr_t Stand_getGlobalPtr;
static worldGetAllVehicles_t Stand_worldGetAllVehicles;
static worldGetAllPeds_t Stand_worldGetAllPeds;
static worldGetAllObjects_t Stand_worldGetAllObjects;
static worldGetAllPickups_t Stand_worldGetAllPickups;
static getScriptHandleBaseAddress_t Stand_getScriptHandleBaseAddress;
static getGameVersion_t Stand_getGameVersion;

extern "C" __declspec(dllexport) void SSHV_init(
	createTexture_t createTexture,
	drawTexture_t drawTexture,
	presentCallbackRegister_t presentCallbackRegister,
	presentCallbackUnregister_t presentCallbackUnregister,
	keyboardHandlerRegister_t keyboardHandlerRegister,
	keyboardHandlerUnregister_t keyboardHandlerUnregister,
	scriptWait_t scriptWait,
	scriptRegister_t scriptRegister,
	scriptRegisterAdditionalThread_t scriptRegisterAdditionalThread,
	scriptUnregister_t scriptUnregister,
	scriptUnregister_old_t scriptUnregister_old,
	nativeInit_t nativeInit,
	nativePush64_t nativePush64,
	nativeCall_t nativeCall,
	getGlobalPtr_t getGlobalPtr,
	worldGetAllVehicles_t worldGetAllVehicles,
	worldGetAllPeds_t worldGetAllPeds,
	worldGetAllObjects_t worldGetAllObjects,
	worldGetAllPickups_t worldGetAllPickups,
	getScriptHandleBaseAddress_t getScriptHandleBaseAddress,
	getGameVersion_t getGameVersion
)
{
	Stand_createTexture = createTexture;
	Stand_drawTexture = drawTexture;
	Stand_presentCallbackRegister = presentCallbackRegister;
	Stand_presentCallbackUnregister = presentCallbackUnregister;
	Stand_keyboardHandlerRegister = keyboardHandlerRegister;
	Stand_keyboardHandlerUnregister = keyboardHandlerUnregister;
	Stand_scriptWait = scriptWait;
	Stand_scriptRegister = scriptRegister;
	Stand_scriptRegisterAdditionalThread = scriptRegisterAdditionalThread;
	Stand_scriptUnregister = scriptUnregister;
	Stand_scriptUnregister_old = scriptUnregister_old;
	Stand_nativeInit = nativeInit;
	Stand_nativePush64 = nativePush64;
	Stand_nativeCall = nativeCall;
	Stand_getGlobalPtr = getGlobalPtr;
	Stand_worldGetAllVehicles = worldGetAllVehicles;
	Stand_worldGetAllPeds = worldGetAllPeds;
	Stand_worldGetAllObjects = worldGetAllObjects;
	Stand_worldGetAllPickups = worldGetAllPickups;
	Stand_getScriptHandleBaseAddress = getScriptHandleBaseAddress;
	Stand_getGameVersion = getGameVersion;
}

__declspec(dllexport) int createTexture(const char* texFileName)
{
	return Stand_createTexture(texFileName);
}

__declspec(dllexport) void drawTexture(int id, int index, int level, int time, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float screenHeightScaleFactor, float r, float g, float b, float a)
{
	Stand_drawTexture(id, index, level, time, sizeX, sizeY, centerX, centerY, posX, posY, rotation, screenHeightScaleFactor, r, g, b, a);
}

__declspec(dllexport) void presentCallbackRegister(Stand::AsiPresentCallback cb)
{
	Stand_presentCallbackRegister(cb);
}

__declspec(dllexport) void presentCallbackUnregister(Stand::AsiPresentCallback cb)
{
	Stand_presentCallbackUnregister(cb);
}

__declspec(dllexport) void keyboardHandlerRegister(Stand::KeyboardHandler handler)
{
	Stand_keyboardHandlerRegister(handler);
}

__declspec(dllexport) void keyboardHandlerUnregister(Stand::KeyboardHandler handler)
{
	Stand_keyboardHandlerUnregister(handler);
}

__declspec(dllexport) void scriptWait(DWORD time)
{
	Stand_scriptWait(time);
}

__declspec(dllexport) void scriptRegister(HMODULE module, void(*LP_SCRIPT_MAIN)())
{
	Stand_scriptRegister(module, LP_SCRIPT_MAIN);
}

__declspec(dllexport) void scriptRegisterAdditionalThread(HMODULE module, void(*LP_SCRIPT_MAIN)())
{
	Stand_scriptRegisterAdditionalThread(module, LP_SCRIPT_MAIN);
}

__declspec(dllexport) void scriptUnregister(HMODULE module)
{
	Stand_scriptUnregister(module);
}

__declspec(dllexport) void scriptUnregister(void(*LP_SCRIPT_MAIN)())
{
	Stand_scriptUnregister_old(LP_SCRIPT_MAIN);
}

__declspec(dllexport) void nativeInit(UINT64 hash)
{
	Stand_nativeInit(hash);
}

__declspec(dllexport) void nativePush64(UINT64 val)
{
	Stand_nativePush64(val);
}

__declspec(dllexport) PUINT64 nativeCall()
{
	return Stand_nativeCall();
}

__declspec(dllexport) UINT64* getGlobalPtr(int globalId)
{
	return Stand_getGlobalPtr(globalId);
}

__declspec(dllexport) int worldGetAllVehicles(int* arr, int arrSize)
{
	return Stand_worldGetAllVehicles(arr, arrSize);
}

__declspec(dllexport) int worldGetAllPeds(int* arr, int arrSize)
{
	return Stand_worldGetAllPeds(arr, arrSize);
}

__declspec(dllexport) int worldGetAllObjects(int* arr, int arrSize)
{
	return Stand_worldGetAllObjects(arr, arrSize);
}

__declspec(dllexport) int worldGetAllPickups(int* arr, int arrSize)
{
	return Stand_worldGetAllPickups(arr, arrSize);
}

__declspec(dllexport) BYTE* getScriptHandleBaseAddress(int handle)
{
	return Stand_getScriptHandleBaseAddress(handle);
}

__declspec(dllexport) eGameVersion getGameVersion()
{
	return Stand_getGameVersion();
}
