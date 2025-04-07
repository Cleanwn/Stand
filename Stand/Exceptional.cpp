#include "Exceptional.hpp"

#include <fmt/core.h>

#include <soup/ObfusString.hpp>
#include <soup/os.hpp>
#include <soup/unicode.hpp>
#include <soup/x64.hpp>

#include <rtcapi.h>
#include <signal.h>
#include <winternl.h>

#include "atStringHash.hpp"
#include "FileLogger.hpp"
#include "Hooking.hpp"
#include "lang.hpp"
#include "mystackwalker.hpp"
#include "pointers.hpp"
#include "PointerNames.hpp"
#include "ScriptVmErrorHandling.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"
#include "yeet_flow.hpp"

namespace Stand
{
	void Exceptional::hideThread(HANDLE hThread) noexcept
	{
#ifndef STAND_DEBUG
		using pNtSetInformationThread = NTSTATUS(NTAPI*)(HANDLE, UINT, PVOID, ULONG);
		auto NtSIT = (pNtSetInformationThread)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtSetInformationThread");
		if (NtSIT != nullptr)
		{
			NtSIT(hThread, 0x11, nullptr, 0);
		}
#endif
	}

	HANDLE Exceptional::createThread(std::function<void()>&& func) noexcept
	{
		auto* fp = new std::function<void()>(std::move(func));
		auto th = CreateThread(nullptr, 0, [](PVOID handover) -> DWORD
		{
			// Set exception handling (This is program-wide, but co-loads exist)
			setUnhandledExceptionHandler();
			_set_abort_behavior(0, 0);
			_RTC_SetErrorFuncW([](int, const wchar_t*, int, const wchar_t*, const wchar_t*, ...)
			{
				Exceptional::report("CRT Run-Time Error", {});
				return 0;
			});
			/*_set_invalid_parameter_handler([](wchar_t const*, wchar_t const*, wchar_t const*, unsigned int, uintptr_t)
			{
				Exceptional::report("Invalid Parameter", {});
			});*/
			_set_purecall_handler([]
			{
				Exceptional::report("Pure Virtual Call", {});
			});
			signal(SIGINT, [](int signal)
			{
				throw std::exception("SIGINT");
			});
			signal(SIGTERM, [](int signal)
			{
				throw std::exception("SIGTERM");
			});
			signal(SIGABRT, [](int signal)
			{
				if (!g_yeeting_with_abort)
				{
					throw std::exception("SIGABRT");
				}
			});
			set_terminate([]
			{
				Exceptional::report("Terminate Called", {});
				abort();
			});

			// Actual thread func
			auto* fp = (std::function<void()>*)handover;
			(*fp)();
			delete fp;
			return 0;
		}, fp, 0, nullptr);
		hideThread(th);
		return th;
	}

	HANDLE Exceptional::createExceptionalThread(std::function<void()>&& func) noexcept
	{
		return createThread([func{ std::move(func) }]
		{
			__try
			{
				func();
			}
			__EXCEPTIONAL()
			{
			}
		});
	}

	void Exceptional::createManagedThread(std::function<void()>&& func) noexcept
	{
		auto th = createThread(std::move(func));
		if (th)
		{
			CloseHandle(th);
		}
	}

	void Exceptional::createManagedCountedThread(DebugString name, std::function<void()>&& func) noexcept
	{
		SOUP_ASSERT(++counted_threads != 0);
#ifdef STAND_DEBUG
		bool contains;
		EXCEPTIONAL_LOCK(counted_thread_names_mtx)
			contains = counted_thread_names.contains(name.data);
			counted_thread_names.emplace(name.data);
		EXCEPTIONAL_UNLOCK(counted_thread_names_mtx)
		if (contains)
		{
			report(fmt::format("Created thread with duplicate name: {}", name.data));
		}
#endif
		createManagedThread([name, func{ std::move(func) }]
		{
			func();
			--counted_threads;
#ifdef STAND_DEBUG
			EXCEPTIONAL_LOCK(counted_thread_names_mtx)
				counted_thread_names.erase(name.data);
			EXCEPTIONAL_UNLOCK(counted_thread_names_mtx)
#endif
		});
	}

	void Exceptional::createManagedExceptionalThread(DebugString name, std::function<void()>&& func) noexcept
	{
		SOUP_ASSERT(++counted_threads != 0);
#ifdef STAND_DEBUG
		bool contains;
		EXCEPTIONAL_LOCK(counted_thread_names_mtx)
			contains = counted_thread_names.contains(name.data);
			counted_thread_names.emplace(name.data);
		EXCEPTIONAL_UNLOCK(counted_thread_names_mtx)
		if (contains)
		{
			//report(fmt::format("Created thread with duplicate name: {}", name.data));
		}
#endif
		createManagedThread([name, func{ std::move(func) }]
		{
			__try
			{
				func();
			}
			__EXCEPTIONAL()
			{
			}
			--counted_threads;
#ifdef STAND_DEBUG
			EXCEPTIONAL_LOCK(counted_thread_names_mtx)
				counted_thread_names.erase(name.data);
			EXCEPTIONAL_UNLOCK(counted_thread_names_mtx)
#endif
		});
	}

	LPTOP_LEVEL_EXCEPTION_FILTER Exceptional::setUnhandledExceptionHandler()
	{
		return SetUnhandledExceptionFilter(reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER>(&::exceptional_on_uncaught_exception));
	}

	void Exceptional::disableExceptionHandling() noexcept
	{
		::disable_exception_handling();
		if (thread_can_run && thread_is_running)
		{
			thread_can_run = false;
			while (thread_is_running);
		}
	}

	void Exceptional::thread_func() noexcept
	{
		THREAD_NAME("Exceptional");
		while (thread_can_run)
		{
			if (exceptional_has_exp())
			{
				void* data;
				handle_exception_t handler;
				exceptional_get_exp(&data, &handler);
				__try
				{
					exceptional_set_retval(handler(data));
				}
				__except (handleExceptionInErrorReporting(GetExceptionInformation()))
				{
					exceptional_set_retval(EXCEPTION_EXECUTE_HANDLER);
				}
				exceptional_clear_exp();
				if (handler != (void*)&handleUncaughtException)
				{
					delete (ExceptionData*)data;
				}
			}
			soup::os::sleep(1);
		}
		thread_is_running = false;
	}

	static Spinlock report_mtx{};

	[[nodiscard]] static std::string addSpears(std::string str)
	{
		str.insert(0, 1, ' ');
		str.insert(0, 1, '-');
		str.insert(0, 1, '-');
		str.insert(0, 1, '-');
		str.insert(0, 1, '-');
		str.push_back(' ');
		str.push_back('-');
		str.push_back('-');
		str.push_back('-');
		str.push_back('-');
		return str;
	}

	void Exceptional::report(const std::string& type, std::string&& message, const ErrorInfo& err, void custom_footer_info(), ExceptionContext ctx) noexcept
	{
		report_mtx.lock();
		if (!g_logger.isInited())
		{
			g_logger.init(FileLogger::getMainFilePath());
		}
		g_logger.log(addSpears(LANG_GET("ERR_L_B")));
		g_logger.enterBlockMode();
		__try
		{
			g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Type: {}").str()), type));
			if (!message.empty())
			{
				g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Message: {}").str()), message));
				message.push_back(' ');
				message.append(LANG_GET("ERR_T_GEN"));
				__try
				{
					Util::toast(std::move(message), TOAST_ABOVE_MAP);
				}
				__except (handleExceptionInErrorReporting(GetExceptionInformation(), "Failed to make exception toast: "))
				{
				}
			}
			if (ctx == ExceptionContext::LUA)
			{
				g_logger.log(LANG_GET("ERR_LUA"));
			}
			g_logger.log(soup::ObfusString("Stack Trace:").str());
			MyStackWalker::getInstance()->generateStackTrace(err, "  ");

			const char* script = g_hooking.executing_script_name;
			if (script != nullptr)
			{
				g_logger.log(soup::ObfusString("Script VM:").str());
				g_logger.log(std::move(soup::ObfusString("  Executing Script: ").str().append(script)));
				g_logger.log(std::move(soup::ObfusString("  Executing Operation: ").str().append(ScriptVmErrorHandling::getOperationName())));
				auto event = hooks::getLastScriptEvent(rage::atStringHash(script));
				if (event.first != 0)
				{
					AbstractPlayer sender(event.second[1 * 2]);
					if (sender.isValid())
					{
						g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("  Last Event: from {}: {}").str()), sender.getName(), hooks::dumpScriptEvent(event)));
					}
				}
			}

			if (custom_footer_info != nullptr)
			{
				custom_footer_info();
			}
		}
		__except (handleExceptionInErrorReporting(GetExceptionInformation()))
		{
		}
		g_logger.leaveBlockMode();
		g_logger.log(addSpears(LANG_GET("ERR_L_E")));
		report_mtx.unlock();
	}

	void Exceptional::report(const std::string& type, std::string&& message, ExceptionData* data, void custom_footer_info()) noexcept
	{
		return report(type, std::move(message), data->exp, custom_footer_info, data->ctx);
	}

	void Exceptional::report(const std::string& type, const ErrorInfo& err) noexcept
	{
		return report(type, LANG_GET("ERR_T_OTH"), err);
	}

	long Exceptional::handleCaughtException(ExceptionData* data) noexcept
	{
		auto exception_name = getExceptionName(data);
		exception_name.insert(0, "Caught ");
		report(std::move(exception_name), LANG_GET("ERR_T_CAU"), data);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	static const char* uncaught_action;

	[[nodiscard]] static bool tryContinueExecution(_EXCEPTION_POINTERS* exp)
	{
		if (exp->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
		{
			uncaught_action = "Letting exception continue search because it's a breakpoint.";
			return false;
		}

		if (exp->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || exp->ExceptionRecord->ExceptionInformation[0] != 8) // Not DEP?
		{
			__try
			{
				auto rip = (const uint8_t*)exp->ContextRecord->Rip;
				const uint8_t* start = rip;
				const uint8_t* code = rip;
				auto ins = soup::x64Disasm(code);
				if (!ins.isValid())
				{
					uncaught_action = "Letting exception continue search because the instruction is unsupported.";
					return false;
				}
				if (strcmp(ins.operation->name, "jmp") == 0)
				{
					// When phConstraintPrismatic fails to allocate its rotation constraint,
					// DisableManifolds will try to `jmp qword ptr [rax+50h]` to call DisableManifolds on the rotation constraint.
					// `rax` is 0 is in this case, so we have an access violation reading from 50h.
					// I could hook rage::phConstraintPrismatic::DisableManifolds for a "proper" patch, but this works.
					uncaught_action = "Continuing execution at a ret instruction because the current instruction is a jump.";
					exp->ContextRecord->Rip = reinterpret_cast<DWORD64>(pointers::nullsub);
				}
				else
				{
					uncaught_action = "Continuing execution at next instruction.";
					exp->ContextRecord->Rip += (code - start);

					if (strcmp(ins.operation->name, "mov") == 0)
					{
						// Carry out instruction as if RHS was 0, e.g. `mov rax, [rbx+10h]` -> `mov rax, 0`.
						// This can help break out of infinite loops, e.g. rage::fwAttachmentEntityExtension::NotifySiblingAttachment.
						switch (ins.operands[0].reg)
						{
						case soup::RA: exp->ContextRecord->Rax = 0; break;
						case soup::RC: exp->ContextRecord->Rcx = 0; break;
						case soup::RD: exp->ContextRecord->Rdx = 0; break;
						case soup::RB: exp->ContextRecord->Rbx = 0; break;
						case soup::SP: exp->ContextRecord->Rsp = 0; break;
						case soup::BP: exp->ContextRecord->Rbp = 0; break;
						case soup::SI: exp->ContextRecord->Rsi = 0; break;
						case soup::DI: exp->ContextRecord->Rdi = 0; break;
						case soup::R8: exp->ContextRecord->R8 = 0; break;
						case soup::R9: exp->ContextRecord->R9 = 0; break;
						case soup::R10: exp->ContextRecord->R10 = 0; break;
						case soup::R11: exp->ContextRecord->R11 = 0; break;
						case soup::R12: exp->ContextRecord->R12 = 0; break;
						case soup::R13: exp->ContextRecord->R13 = 0; break;
						case soup::R14: exp->ContextRecord->R14 = 0; break;
						case soup::R15: exp->ContextRecord->R15 = 0; break;
						}
					}
				}
				return true;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}

		uncaught_action = "Continuing execution at a ret instruction because the current instruction is inaccessible.";
		exp->ContextRecord->Rip = reinterpret_cast<DWORD64>(pointers::nullsub);
		return true;
	}

	long Exceptional::handleUncaughtException(_EXCEPTION_POINTERS* exp) noexcept
	{
		const long ret = tryContinueExecution(exp) ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;

		auto exception_name = getExceptionName(exp);
		exception_name.insert(0, "Uncaught ");
		report(std::move(exception_name), LANG_GET("ERR_T_GME"), exp, []
		{
			std::string model_request_log = {};
			const time_t deadline = get_current_time_millis() - 1'000;
			for (const auto& entry : g_hooking.first_model_info_request_times)
			{
				if (entry.second >= deadline)
				{
					StringUtils::list_append(model_request_log, Util::to_padded_hex_string_with_0x(entry.first));
				}
			}
			if (!model_request_log.empty())
			{
				g_logger.log(model_request_log.insert(0, "New models in the last second: "));
			}

			g_logger.log(std::move(std::string("Action: ").append(uncaught_action)));
		});

		return ret;
	}

	static void parseExceptionInformation(std::string& exception_name, ULONG_PTR info[15])
	{
		switch (info[0])
		{
		default:
			exception_name.append(" while ");
			exception_name.append(fmt::to_string(info[0]));
			exception_name.append(" at ");
			exception_name.append(PointerNames::format((void*)info[1]));
			break;

		case 0:
			exception_name.append(" while reading from ");
			exception_name.append(PointerNames::format((void*)info[1]));
			break;

		case 1:
			exception_name.append(" while writing to ");
			exception_name.append(PointerNames::format((void*)info[1]));
			break;

		case 8:
			exception_name.append(" (DEP at ");
			exception_name.append(PointerNames::format((void*)info[1]));
			exception_name.push_back(')');
			break;
		}
	}

	std::string Exceptional::getExceptionName(_EXCEPTION_POINTERS* exp) noexcept
	{
		std::string exception_name;
		switch (exp->ExceptionRecord->ExceptionCode)
		{
		default:
			exception_name = Util::to_padded_hex_string((uint32_t)exp->ExceptionRecord->ExceptionCode);
			break;

		case 0xE06D7363:
			__try
			{
				exception_name = reinterpret_cast<std::exception*>(exp->ExceptionRecord->ExceptionInformation[1])->what();
				exception_name = StringUtils::utf16_to_utf8(soup::unicode::acp_to_utf16(exception_name));
				exception_name.insert(0, 1, '"');
				exception_name.push_back('"');
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				exception_name = "C++ exception";
			}
			break;

		case EXCEPTION_ACCESS_VIOLATION:
			exception_name = "ACCESS_VIOLATION";
			parseExceptionInformation(exception_name, exp->ExceptionRecord->ExceptionInformation);
			break;

		case EXCEPTION_IN_PAGE_ERROR:
			exception_name = "IN_PAGE_ERROR";
			parseExceptionInformation(exception_name, exp->ExceptionRecord->ExceptionInformation);
			exception_name.append(" (Status ").append(fmt::to_string(exp->ExceptionRecord->ExceptionInformation[2])).push_back(')');
			break;

		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			exception_name = "ARRAY_BOUNDS_EXCEEDED";
			break;

		case EXCEPTION_BREAKPOINT:
			exception_name = "BREAKPOINT";
			break;

		case EXCEPTION_DATATYPE_MISALIGNMENT:
			exception_name = "DATATYPE_MISALIGNMENT";
			break;

		case EXCEPTION_FLT_DENORMAL_OPERAND:
			exception_name = "FLT_DENORMAL_OPERAND";
			break;

		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			exception_name = "FLT_DIVIDE_BY_ZERO";
			break;

		case EXCEPTION_FLT_INEXACT_RESULT:
			exception_name = "FLT_INEXACT_RESULT";
			break;

		case EXCEPTION_FLT_INVALID_OPERATION:
			exception_name = "FLT_INVALID_OPERATION";
			break;

		case EXCEPTION_FLT_OVERFLOW:
			exception_name = "FLT_OVERFLOW";
			break;

		case EXCEPTION_FLT_STACK_CHECK:
			exception_name = "FLT_STACK_CHECK";
			break;

		case EXCEPTION_FLT_UNDERFLOW:
			exception_name = "FLT_UNDERFLOW";
			break;

		case EXCEPTION_ILLEGAL_INSTRUCTION:
			exception_name = "ILLEGAL_INSTRUCTION";
			break;

		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			exception_name = "INT_DIVIDE_BY_ZERO";
			break;

		case EXCEPTION_INT_OVERFLOW:
			exception_name = "INT_OVERFLOW";
			break;

		case EXCEPTION_INVALID_DISPOSITION:
			exception_name = "INVALID_DISPOSITION";
			break;

		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			exception_name = "NONCONTINUABLE_EXCEPTION";
			break;

		case EXCEPTION_PRIV_INSTRUCTION:
			exception_name = "PRIV_INSTRUCTION";
			break;

		case EXCEPTION_SINGLE_STEP:
			exception_name = "SINGLE_STEP";
			break;

		case EXCEPTION_STACK_OVERFLOW:
			exception_name = "STACK_OVERFLOW";
			break;
		}
		return exception_name;
	}

	std::string Exceptional::getExceptionName(const ExceptionData* data) noexcept
	{
		return getExceptionName(data->exp);
	}

	long Exceptional::handleExceptionInErrorReporting(_EXCEPTION_POINTERS* exp, std::string msg) noexcept
	{
		msg.append(getExceptionName(exp));
		g_logger.log(std::move(msg));

		return EXCEPTION_EXECUTE_HANDLER;
	}
}
