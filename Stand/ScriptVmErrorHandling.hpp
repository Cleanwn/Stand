#pragma once

#include <cstdint>
#include <string>

#include "natives_decl.hpp"
#include "script_vm_reimpl.hpp"

namespace Stand
{
	struct ScriptVmErrorHandling
	{
#if SVM_REIMPL
		inline static uint8_t reimpl_last_opcode;
		inline static rage::scrNativeHandler reimpl_last_native_entrypoint;
#endif

		[[nodiscard]] static uint8_t getLastOpCode();
		[[nodiscard]] static rage::scrNativeHandler getLastNativeEntrypoint();

		[[nodiscard]] static std::string getOperationName();

		static uint32_t __fastcall onScriptThreadError();
		static uint32_t __fastcall onScriptThreadErrorInternal(uint64_t stack_ptr, uint64_t frame_ptr, bool reimpl = true);
		static void __fastcall onInvalidScriptOpCode(int64_t opcode);

		static void onHadToKillfreemode();
		static void restartFreemode();
	};
}
