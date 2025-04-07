#include "asi_api.hpp"

#include "AbstractEntity.hpp"
#include "NativeCallContext.hpp"
#include "pointers.hpp"
#include "Renderer.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptMgr.hpp"

#define LOG_SH false

#if LOG_SH
#include "FileLogger.hpp"
#endif

namespace Stand
{
	int createTexture(const char* texFileName)
	{
#if LOG_SH
		g_logger.log("createTexture");
#endif
		return g_renderer.createTexture(texFileName);
	}

	void drawTexture(int id, int index, int level, int time, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float screenHeightScaleFactor, float r, float g, float b, float a)
	{
#if LOG_SH
		g_logger.log("drawTexture");
#endif
		return g_renderer.drawTextureTimedCP(id, time, sizeX, sizeY, centerX, centerY, posX, posY, rotation, r, g, b, a);
	}

	void presentCallbackRegister(AsiPresentCallback cb)
	{
#if LOG_SH
		g_logger.log("presentCallbackRegister");
#endif
		g_renderer.asi_present_callbacks.insert(cb);
	}

	void presentCallbackUnregister(AsiPresentCallback cb)
	{
#if LOG_SH
		g_logger.log("presentCallbackUnregister");
#endif
		g_renderer.asi_present_callbacks.erase(cb);
	}

	void keyboardHandlerRegister(KeyboardHandler handler)
	{
#if LOG_SH
		g_logger.log("keyboardHandlerRegister");
#endif
		g_renderer.keyboard_handlers.insert(handler);
	}

	void keyboardHandlerUnregister(KeyboardHandler handler)
	{
#if LOG_SH
		g_logger.log("keyboardHandlerUnregister");
#endif
		g_renderer.keyboard_handlers.erase(handler);
	}

	void scriptWait(DWORD time)
	{
#if LOG_SH
		g_logger.log("scriptWait");
#endif
		Script::current()->yield(time);
	}

	void scriptRegister(HMODULE module, void(*LP_SCRIPT_MAIN)())
	{
#if LOG_SH
		g_logger.log("scriptRegister");
#endif
		if (g_asi_main_script_registered)
		{
			throw std::exception("scriptRegister was called twice. Use scriptRegisterAdditionalThread if this is the intended behaviour.");
		}
		g_asi_main_script_registered = true;
		scriptRegisterAdditionalThread(module, LP_SCRIPT_MAIN);
	}

	void scriptRegisterAdditionalThread(HMODULE module, void(*LP_SCRIPT_MAIN)())
	{
		g_script_mgr.addScript(module, std::make_unique<Script>(LP_SCRIPT_MAIN));
	}

	void scriptUnregister(HMODULE module)
	{
#if LOG_SH
		g_logger.log("scriptUnregister");
#endif
		g_script_mgr.removeScripts(module);
	}

	void scriptUnregister_old(void(*LP_SCRIPT_MAIN)())
	{
#if LOG_SH
		g_logger.log("scriptUnregister_old");
#endif
		g_script_mgr.removeScript(LP_SCRIPT_MAIN);
	}

	inline static XoredNativeHash current_call_native_hash;
	inline static NativeCallContext native_call_ctx = NativeCallContext();

	void nativeInit(UINT64 hash)
	{
#if LOG_SH
		g_logger.log("nativeInit");
#endif
		current_call_native_hash = XoredNativeHash(hash);
		native_call_ctx.reset();
	}

	void nativePush64(UINT64 val)
	{
#if LOG_SH
		g_logger.log("nativePush64");
#endif
		native_call_ctx.pushArg(val);
	}

	PUINT64 nativeCall()
	{
#if LOG_SH
		g_logger.log("nativeCall");
#endif
		if (current_call_native_hash == XoredNativeHash(0x9DE624D2FC4B603F))
		{
			// NETWORK_IS_SESSION_STARTED
			if (AsiSettings::deceive_online_checks)
			{
				native_call_ctx.setReturnValue<BOOL>(false);
				return native_call_ctx.getReturnValuePtr<UINT64>();
			}
		}
		native_call_ctx.invoke(current_call_native_hash);
		return native_call_ctx.getReturnValuePtr<UINT64>();
	}

	UINT64* getGlobalPtr(int globalId)
	{
#if LOG_SH
		g_logger.log("getGlobalPtr");
#endif
		return ScriptGlobal(globalId).as<UINT64*>();
	}

	int worldGetAllVehicles(int* arr, int arrSize)
	{
#if LOG_SH
		g_logger.log("worldGetAllVehicles");
#endif
		int i = 0;
		AbstractEntity::getAllVehicles([&](AbstractEntity&& ent)
		{
			arr[i] = ent.getHandle();
			return ++i < arrSize;
		});
		return i;
	}

	int worldGetAllPeds(int* arr, int arrSize)
	{
#if LOG_SH
		g_logger.log("worldGetAllPeds");
#endif
		int i = 0;
		AbstractEntity::getAllPeds([&](AbstractEntity&& ent)
		{
			arr[i] = ent.getHandle();
			return ++i < arrSize;
		});
		return i;
	}

	int worldGetAllObjects(int* arr, int arrSize)
	{
#if LOG_SH
		g_logger.log("worldGetAllObjects");
#endif
		int i = 0;
		AbstractEntity::getAllObjects([&](AbstractEntity&& ent)
		{
			arr[i] = ent.getHandle();
			return ++i < arrSize;
		});
		return i;
	}

	int worldGetAllPickups(int* arr, int arrSize)
	{
#if LOG_SH
		g_logger.log("worldGetAllPickups");
#endif
		int i = 0;
		AbstractEntity::getAllPickups([&](AbstractEntity&& ent)
		{
			arr[i] = ent.getHandle();
			return ++i < arrSize;
		});
		return i;
	}

	BYTE* getScriptHandleBaseAddress(int handle)
	{
#if LOG_SH
		g_logger.log("getScriptHandleBaseAddress");
#endif
		return (BYTE*)AbstractEntity::get(handle).getPointer();
	}

	eGameVersion getGameVersion()
	{
#if LOG_SH
		g_logger.log("getGameVersion");
#endif
		return VER_LATEST;
	}
}
