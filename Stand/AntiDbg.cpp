#include "AntiDbg.hpp"

#undef UNICODE
#include <tlhelp32.h>

#include <soup/ObfusString.hpp>
#include <soup/os.hpp>

#include "atStringHash.hpp"
#include "Codename.hpp"
#include "PointerCache.hpp"
#include "FileLogger.hpp"

namespace Stand
{
	void AntiDbg::apologiseInAdvance(int code)
	{
		g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("I'm sorry, Dave. I'm afraid I can't let you do that ({}).").str()), code));
	}

	void AntiDbg::enqueueSuspension(uintptr_t data)
	{
		if (!PointerCache::isInited())
		{
			PointerCache::init();
			PointerCache::cache.emplace(Codename("AZ").toString(), data ^ PointerCache::xor_magic);
			PointerCache::saveAndDeinit(); // keep deinited so mainRunningLoop doesn't pop it immediately after
		}
		else
		{
			PointerCache::cache.emplace(Codename("AZ").toString(), data ^ PointerCache::xor_magic);
			PointerCache::save();
		}
	}

	void AntiDbg::bToTheSToTheOToTheD()
	{
		auto ntdll = LoadLibraryA(soup::ObfusString("ntdll.dll"));

		using NtRaiseHardError_t = NTSTATUS(NTAPI*)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
		using RtlAdjustPrivilege_t = NTSTATUS(NTAPI*)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

		auto RtlAdjustPrivilege = (RtlAdjustPrivilege_t)GetProcAddress(ntdll, soup::ObfusString("RtlAdjustPrivilege"));
		auto NtRaiseHardError = (NtRaiseHardError_t)GetProcAddress(ntdll, soup::ObfusString("NtRaiseHardError"));

		// Enable SeShutdownPrivilege
		BOOLEAN bEnabled;
		RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);

		ULONG uResp;
		NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &uResp);
	}

	uint8_t AntiDbg::getRunningDebuggerId(bool passive) // OBFUS!
	{
		if constexpr (isEnabled())
		{
			if (soup::os::getCurrentPeb()->BeingDebugged)
			{
				apologiseInAdvance(2);
				if (!passive)
				{
					enqueueSuspension(2);
					AntiDbg::bToTheSToTheOToTheD();
				}
				crash();
				return 51;
			}

			{
				uint8_t running_debugger_id = 0;
				HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
				PROCESSENTRY32 entry;
				entry.dwSize = sizeof(PROCESSENTRY32);
				if (Process32First(snapshot, &entry))
				{
					do
					{
						if (entry.th32ProcessID != GetCurrentProcessId())
						{
							switch (rage::atStringHash(entry.szExeFile))
							{
							case ATSTRINGHASH("IMMUNITYDEBUGGER.EXE"):
								running_debugger_id = 1;
								break;
							case ATSTRINGHASH("ImportREC.exe"):
								running_debugger_id = 2;
								break;
							case ATSTRINGHASH("ollydbg.exe"):
								running_debugger_id = 3;
								break;
							case ATSTRINGHASH("protection_id.exe"):
								running_debugger_id = 4;
								break;
							case ATSTRINGHASH("reshacker.exe"):
								running_debugger_id = 5;
								break;
							case ATSTRINGHASH("scylla.exe"):
								running_debugger_id = 6;
								break;
							case ATSTRINGHASH("scylla_x64.exe"):
								running_debugger_id = 7;
								break;
							case ATSTRINGHASH("scylla_x86.exe"):
								running_debugger_id = 8;
								break;
							case ATSTRINGHASH("Task Explorer-x64.exe"):
								running_debugger_id = 9;
								break;
							case ATSTRINGHASH("windbg.exe"):
								running_debugger_id = 10;
								break;
							case ATSTRINGHASH("x32dbg.exe"):
								running_debugger_id = 11;
								break;
							case ATSTRINGHASH("x64dbg.exe"):
								running_debugger_id = 12;
								break;
							case ATSTRINGHASH("pd32.exe"):
								running_debugger_id = 121;
								break;
							case ATSTRINGHASH("pd64.exe"):
								running_debugger_id = 122;
								break;
							}
						}
					} while (Process32Next(snapshot, &entry));
					CloseHandle(snapshot);
					if (running_debugger_id != 0)
					{
						return running_debugger_id;
					}
				}
			}

			decltype(&::FindWindowA) fpFindWindowA;
			{
				fpFindWindowA = reinterpret_cast<decltype(&::FindWindowA)>(GetProcAddress(GetModuleHandleA(soup::ObfusString("user32.dll")), soup::ObfusString("FindWindowA")));
			}
			if (fpFindWindowA == nullptr)
			{
				return 50;
			}

#define BAD_WINDOW(i, name) { if (fpFindWindowA(nullptr, soup::ObfusString(name))) return 30 + i; }
			BAD_WINDOW(0, "OLLYDBG");
			BAD_WINDOW(1, "disassembly");
			BAD_WINDOW(2, "[CPU");
			BAD_WINDOW(3, "Immunity");
			BAD_WINDOW(4, "WinDbg");
			BAD_WINDOW(5, "Import reconstructor");
			BAD_WINDOW(6, "x32dbg");
			BAD_WINDOW(7, "x64dbg");
			BAD_WINDOW(8, "scylla");
		}

		last_debugger_check = get_current_time_millis();
		return 0;
	}
}
