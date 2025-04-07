#include "ScriptVmErrorHandling.hpp"

#include <fmt/core.h>

#include "atStringHash.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "fmt_arg.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "ScriptErrorRecoveryMode.hpp"
#include "ScriptGlobal.hpp"
#include "script_program.hpp"
#include "script_thread.hpp"
#include "script_vm_common.hpp"
#include "TransitionState.hpp"
#include "Util.hpp"

#ifdef STAND_DEBUG
#include "FileLogger.hpp"
#endif

namespace Stand
{
	extern "C"
	{
		// script_vm_helper.asm
		uint8_t script_vm_helper_get_last_opcode();
		rage::scrNativeHandler script_vm_get_op_native_executing();
	}

	uint8_t ScriptVmErrorHandling::getLastOpCode()
	{
#if SVM_REIMPL
		if (g_use_script_vm_reimpl)
		{
			return reimpl_last_opcode;
		}
#endif
		return script_vm_helper_get_last_opcode();
	}

	rage::scrNativeHandler ScriptVmErrorHandling::getLastNativeEntrypoint()
	{
#if SVM_REIMPL
		if (g_use_script_vm_reimpl)
		{
			return reimpl_last_native_entrypoint;
		}
#endif
		return script_vm_get_op_native_executing();
	}

	std::string ScriptVmErrorHandling::getOperationName()
	{
		const auto last_opcode = getLastOpCode();
		std::string name(opcode_names[last_opcode]);
		if (last_opcode == Native)
		{
			name.push_back(' ');
			name.append(native_entrypoint_to_string(getLastNativeEntrypoint()));
		}
		return name;
	}

	static void addDevBuildInfoToMsg(const rage::scrThread* thread, std::string& msg)
	{
#ifdef STAND_DEV
		msg.append(soup::ObfusString(" (ip = ").str());
		msg.append(fmt::to_string(thread->m_context.m_instruction_pointer));
		msg.append(soup::ObfusString(", within func_").str());
		auto program = rage::scrProgram::fromHash(thread->m_context.m_script_hash);
		msg.append(fmt::to_string(program->getFuncIndexByCodeIndex(thread->m_context.m_instruction_pointer) - 1));
		msg.push_back(')');
#endif
	}

	uint32_t __fastcall ScriptVmErrorHandling::onScriptThreadError()
	{
		return onScriptThreadErrorInternal(
			script_vm_helper_get_last_stack_ptr(),
			script_vm_helper_get_last_frame_ptr(),
			false
		);
	}

	uint32_t __fastcall ScriptVmErrorHandling::onScriptThreadErrorInternal(uint64_t stack_ptr, uint64_t frame_ptr, bool reimpl)
	{
		auto* thread = rage::scrThread::get();
		__try
		{
			// instruction pointer is set before killing the thread
			thread->m_context.m_stack_pointer = uint32_t((stack_ptr - (uint64_t)thread->m_stack) / 8) + 1;
			thread->m_context.m_frame_pointer = uint32_t((frame_ptr - (uint64_t)thread->m_stack) / 8);
			const auto last_opcode = getLastOpCode();
			/*g_logger.log(fmt::format("last opcode: {}", last_opcode));
			for (int i = -5; i <= 5; i++)
			{
				g_logger.log(fmt::format("{}: {}", i, *(thread->m_context.getCodePtr() + i)));
			}*/
			if (thread->m_context.m_script_hash == ATSTRINGHASH("valentinerpreward2"))
			{
				thread->m_context.m_state = rage::scrThread::ABORTED;
				//g_logger.log(fmt::format("{} script crashed on {} (opcode {}).", thread->m_name, opcode_names[last_opcode], last_opcode));
				return 2;
			}
#ifdef STAND_DEBUG
			const char* not_recovered_reason;
#endif
			bool best_effort = false;
			switch (script_error_recovery_mode)
			{
			case ScriptErrorRecoveryMode::FREEMODE_ONLY:
				if (thread->m_context.m_script_hash == ATSTRINGHASH("freemode"))
				{
					break;
				}
				[[fallthrough]];
			case ScriptErrorRecoveryMode::DISABLED:
#ifdef STAND_DEBUG
				not_recovered_reason = "error recovery is disabled";
#endif
				goto let_it_die;
			}
			switch (last_opcode)
			{
			default:
#ifdef STAND_DEBUG
				not_recovered_reason = "opcode is not supported";
#endif
				best_effort = true;
			let_it_die:
				thread->m_context.m_state = rage::scrThread::ABORTED;
				{
					const bool is_freemode = thread->m_context.m_script_hash == ATSTRINGHASH("freemode");
					if (last_opcode != Return || is_freemode)
					{
#ifdef STAND_DEBUG
						g_logger.log(fmt::format("{} was left for dead because {}", thread->m_name, not_recovered_reason));
#endif
						std::string msg = LANG_FMT("SVM_DIE", FMT_ARG("script", thread->m_name), FMT_ARG("operation", getOperationName()));
						addDevBuildInfoToMsg(thread, msg);
						hooks::blameScriptCrash(thread, msg, true);
						Util::toast(std::move(msg), TOAST_ALL);
						if (is_freemode
							&& best_effort
							&& last_opcode != Return
							)
						{
							onHadToKillfreemode();
						}
					}
				}
				return 2;

			case pArray1:
				++thread->m_context.m_instruction_pointer;
				if (!reimpl)
				{
					--thread->m_context.m_stack_pointer;
				}
				thread->getStackPtrAsScrValue()[0].Reference = (thread->getStackPtrAsScrValue()[1].Reference + 1);
				break;

			case ArrayGet1:
				++thread->m_context.m_instruction_pointer;
				if (!reimpl)
				{
					--thread->m_context.m_stack_pointer;
				}
				thread->getStackPtrAsScrValue()[0].Any = (thread->getStackPtrAsScrValue()[1].Reference + 1)->Any;
				break;

			case ArraySet1:
				thread->m_context.m_instruction_pointer++;
				if (!reimpl)
				{
					thread->m_context.m_stack_pointer -= 3;
				}
				break;

			case pArray2:
				thread->m_context.m_instruction_pointer += 2;
				if (!reimpl)
				{
					--thread->m_context.m_stack_pointer;
				}
				thread->getStackPtrAsScrValue()[0].Reference = (thread->getStackPtrAsScrValue()[1].Reference + 1);
				break;

			case ArrayGet2:
				thread->m_context.m_instruction_pointer += 2;
				if (!reimpl)
				{
					--thread->m_context.m_stack_pointer;
				}
				thread->getStackPtrAsScrValue()[0].Any = (thread->getStackPtrAsScrValue()[1].Reference + 1)->Any;
				break;

			case pCall:
				break;
			}
			std::string msg = LANG_FMT("SVM_ERR", FMT_ARG("script", thread->m_name), FMT_ARG("operation", getOperationName()));
			addDevBuildInfoToMsg(thread, msg);
			hooks::logScriptScrash(thread, std::move(msg));
		}
		__except (hooks::onScriptException(GetExceptionInformation()))
		{
		}
		return rage::scrThread::STAND_ERROR;
	}

	void __fastcall ScriptVmErrorHandling::onInvalidScriptOpCode(int64_t opcode)
	{
		auto* thread = rage::scrThread::get();
		std::string msg = LANG_FMT("SVM_INVOP", FMT_ARG("opcode", opcode), FMT_ARG("script", thread->m_name));
		//addDebugBuildInfoToMsg(thread, msg); // instruction pointer is not updated before calling onInvalidScriptOpCode
		Util::toast(std::move(msg), TOAST_ALL);
		if (thread->m_context.m_script_hash == ATSTRINGHASH("freemode"))
		{
			onHadToKillfreemode();
		}
		thread->m_context.m_state = rage::scrThread::ABORTED;
	}

	void ScriptVmErrorHandling::onHadToKillfreemode()
	{
		// Prevent bail due to dead fm
		//ScriptGlobal(GLOBAL_TRANSITION_STATE).set<int>(TRANSITION_STATE_START_FM_LAUNCH_SCRIPT);

		FiberPool::queueJob(&restartFreemode);
	}

	void ScriptVmErrorHandling::restartFreemode()
	{
		// Request & start maintrans and set thread id global
		SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(ATSTRINGHASH("maintransition"));
		while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(ATSTRINGHASH("maintransition")))
		{
			Script::current()->yield();
		}
		ScriptGlobal(GLOBAL_MAINTRANSID).set<>(SYSTEM::START_NEW_SCRIPT_WITH_NAME_HASH(ATSTRINGHASH("maintransition"), 8032));
		SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(ATSTRINGHASH("maintransition"));

		// Manage trans state so maintrans goes to start fm asap
		ScriptGlobal(GLOBAL_TRANSITION_STATE).set<>(0);
		do
		{
			Script::current()->yield();
			GRAPHICS::RESET_PAUSED_RENDERPHASES();
		} while (ScriptGlobal(GLOBAL_TRANSITION_STATE).get<int>() == 0);
		Script::current()->yield();
		ScriptGlobal(GLOBAL_TRANSITION_STATE).set<>(TRANSITION_STATE_WAIT_JOIN_FM_SESSION);
	}
}
