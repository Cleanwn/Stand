#pragma once

#include <string>
#include <vector>

#include "natives_decl.hpp"

#include "ScriptVmOpCode.hpp"

namespace Stand
{
	extern "C"
	{
		// script_vm_helper.asm
		uint32_t		script_vm_helper_get_last_ins_ptr();
		uint64_t		script_vm_helper_get_last_stack_ptr();
		uint64_t		script_vm_helper_get_last_frame_ptr();
	}

	extern const char* opcode_names[131];

	[[nodiscard]] extern uint32_t getOpcodeArgLength(const uint8_t* op_ptr);

	[[nodiscard]] extern std::string native_entrypoint_to_string(const rage::scrNativeHandler entrypoint);

	[[nodiscard]] extern std::vector<rage::scrNativeHash> native_entrypoint_to_hashes(const rage::scrNativeHandler entrypoint);
}
