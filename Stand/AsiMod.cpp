#include "AsiMod.hpp"

#include <Winternl.h>
#include <TlHelp32.h>

#include <filesystem>

#include <soup/ObfusString.hpp>
#include <soup/rand.hpp>

#include "asi_api.hpp"
#include "bin.hpp"
#include "CommandAsiMods.hpp"
#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"
#include "Hooking.hpp"
#include "main.hpp"
#include "evtPlayerJoinEvent.hpp"
#include "evtPlayerLeaveEvent.hpp"
#include "ScriptMgr.hpp"
#include "StringUtils.hpp"
#include "Tunables.hpp"
#include "Util.hpp"

namespace Stand
{
	AsiMod::AsiMod(CommandPhysical* cmd, AsiDirectory dir)
		: cmd(cmd), dir(dir)
	{
		initCommand();
	}

	void AsiMod::initCommand()
	{
		cmd->command_names = { std::move(CommandName(CMDNAME("asi")).append(cmd->menu_name.getLiteralForCommandName())) };
		StringUtils::simplify(cmd->command_names.at(0));
		cmd->help_text.setLiteral(StringUtils::utf16_to_utf8(getPath(true)));
	}

	std::wstring AsiMod::getPath(bool discrete) const
	{
		std::wstring path;
		switch (dir)
		{
		default:
		case ASIDIR_STAND:
			if (discrete)
			{
				path = L"%appdata%";
			}
			else
			{
				path = _wgetenv(L"appdata");
			}
			path.append(LR"(\Stand\ASI Mods\)");
			break;

		case ASIDIR_GAME:
			path = std::filesystem::current_path().wstring();
			path.push_back(L'\\');
			break;
		}
		return path.append(cmd->menu_name.getLiteralUtf16()).append(L".asi");
	}

	bool AsiMod::isRunning() const noexcept
	{
		return asi_h != nullptr;
	}

	void AsiMod::load()
	{
		auto asi_path = getPath();
		if (!std::filesystem::is_regular_file(asi_path))
		{
			return;
		}

		auto scripthookv_path = std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Bin\)");
		if (!std::filesystem::exists(scripthookv_path))
		{
			std::filesystem::create_directory(scripthookv_path);
		}
		if (use_temp_dir)
		{
			temp_path = std::move(std::wstring(scripthookv_path).append(L"Temp"));
			if (!std::filesystem::exists(temp_path))
			{
				std::filesystem::create_directory(temp_path);
			}
		}
		scripthookv_path.append(L"ScriptHookV.dll");

		Util::bin_file_write(scripthookv_path, bin::scripthookv_dll, sizeof(bin::scripthookv_dll));
		shv_h = LoadLibraryW(scripthookv_path.c_str());
		if (shv_h == nullptr)
		{
			Util::toast(soup::ObfusString("Failed to load ScriptHookV.dll"));
			return;
		}
		((SSHV_init_t)GetProcAddress(shv_h, soup::ObfusString("SSHV_init")))(
			&createTexture,
			&drawTexture,
			&presentCallbackRegister,
			&presentCallbackUnregister,
			&keyboardHandlerRegister,
			&keyboardHandlerUnregister,
			&scriptWait,
			&scriptRegister,
			&scriptRegisterAdditionalThread,
			&scriptUnregister,
			&scriptUnregister_old,
			&nativeInit,
			&nativePush64,
			&nativeCall,
			&getGlobalPtr,
			&worldGetAllVehicles,
			&worldGetAllPeds,
			&worldGetAllObjects,
			&worldGetAllPickups,
			&getScriptHandleBaseAddress,
			&getGameVersion
		);

		if (g_tunables.getBool(TUNAHASH("DISABLE_ASIS")))
		{
			return;
		}

		if (use_temp_dir)
		{
			temp_path.append(L"\\ASI_");
			temp_path.append(soup::rand.str<std::wstring>(5));
			temp_path.append(L".dll");
			//g_logger.log(fmt::format("{} -> {}", asi_path, temp_path));
			std::filesystem::copy(asi_path, temp_path);
		}
		else
		{
			temp_path = asi_path;
		}
		g_asi_main_script_registered = false;
		asi_h = LoadLibraryW(temp_path.c_str());
		if (!asi_h)
		{
			Util::toast(LOC("ASICE"));
		}
	}

	static uint64_t GetThreadStartAddress(__in HANDLE hThread) // by Echo
	{
		using NTQUERYINFOMATIONTHREAD = NTSTATUS(WINAPI*)(HANDLE, LONG, PVOID, ULONG, PULONG);

#define ThreadQuerySetWin32StartAddress 9

		NTSTATUS ntStatus;
		uint64_t threadStartAddr = 0;
		HANDLE hPeusdoCurrentProcess, hNewThreadHandle;
		NTQUERYINFOMATIONTHREAD NtQueryInformationThread;

		if ((NtQueryInformationThread = (NTQUERYINFOMATIONTHREAD)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread")))
		{
			hPeusdoCurrentProcess = GetCurrentProcess();
			if (DuplicateHandle(hPeusdoCurrentProcess, hThread, hPeusdoCurrentProcess, &hNewThreadHandle, THREAD_QUERY_INFORMATION, FALSE, 0))
			{
				ntStatus = NtQueryInformationThread(hNewThreadHandle, ThreadQuerySetWin32StartAddress, &threadStartAddr, sizeof(threadStartAddr), nullptr);
				CloseHandle(hNewThreadHandle);
				if (ntStatus != 0)
				{
					return 0;
				}
			}
		}
		return threadStartAddr;
	}

	void AsiMod::unload(bool notify)
	{
		if (asi_h)
		{
			HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
			if (h != INVALID_HANDLE_VALUE)
			{
				THREADENTRY32 te;
				te.dwSize = sizeof(te);
				if (Thread32First(h, &te))
				{
					do
					{
						HANDLE th = OpenThread(READ_CONTROL | THREAD_TERMINATE, false, te.th32ThreadID);
						HMODULE hmod;
						if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)GetThreadStartAddress(th), &hmod) == TRUE && hmod != nullptr)
						{
							if (hmod == asi_h)
							{
								TerminateThread(th, 0);
							}
						}
						CloseHandle(th);
					} while (Thread32Next(h, &te));
				}
				CloseHandle(h);
			}
			FreeLibrary(asi_h);
			g_script_mgr.removeScripts(asi_h);
			asi_h = nullptr;
			if (GetModuleHandleW(temp_path.c_str()))
			{
				if (notify)
				{
					Util::toast(LOC("ASI_D_T"));
				}
			}
		}
		if (shv_h)
		{
			FreeLibrary(shv_h);
			shv_h = nullptr;
		}
	}
}
