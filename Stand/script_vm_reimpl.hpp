#pragma once

#define SVM_REIMPL true

#if SVM_REIMPL

#include "script_program.hpp"
#include "scrNativeCallContext.hpp"

namespace rage
{
	extern uint32_t script_vm_reimpl(scrValue* stack, scrValue** globals, const scrProgram& pt, scrThreadContext* ser);
}

namespace Stand
{
	inline bool g_use_script_vm_reimpl = true;
}
#endif
