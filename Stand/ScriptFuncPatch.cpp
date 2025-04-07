#include "ScriptFuncPatch.hpp"

#include "script_program.hpp"
#include "ScriptVmOpCode.hpp"

namespace Stand
{
	uint8_t ScriptFuncPatch::patchBytesReturnVoid_0[] = {
		Nop,
		Return, 0, 0,
	};

	uint8_t ScriptFuncPatch::patchBytesReturnVoid_1[] = {
		Nop,
		Return, 1, 0,
	};
	
	uint8_t ScriptFuncPatch::patchBytesReturnFalse_1[] = {
		Nop,
		iPush_0,
		Return, 1, 1,
	};
	
	uint8_t ScriptFuncPatch::patchBytesReturnTrue_1[] = {
		Nop,
		iPush_1,
		Return, 1, 1,
	};

	uint8_t ScriptFuncPatch::patchBytesReturnTrue_2[] = {
		Nop,
		iPush_1,
		Return, 2, 1,
	};

	uint8_t ScriptFuncPatch::patchBytesReturnTrue_3[] = {
		Nop,
		iPush_1,
		Return, 3, 1,
	};

	uint8_t ScriptFuncPatch::patchBytesNop3[] = {
		Nop, Nop, Nop
	};

	void ScriptFuncPatch::onTick(const hash_t script, const uint32_t func_i, uint8_t* const patchBytes, const size_t patchBytesSize, const uint32_t code_offset)
	{
		auto* prgm = rage::scrProgram::fromHash(script);
		if (patch.isPatched())
		{
			if (prgm == nullptr)
			{
				patch.dispose();
			}
		}
		else
		{
			if (prgm != nullptr)
			{
				auto func = prgm->getFunc(func_i);
				SOUP_IF_UNLIKELY (code_offset != 0)
				{
					patch.initPatch(prgm->getCodePtr(func.start + code_offset), patchBytes, patchBytesSize);
				}
				else
				{
					auto fullPatchBytes = new uint8_t[func.size()];
					memcpy(fullPatchBytes, patchBytes, patchBytesSize);
					memset(fullPatchBytes + patchBytesSize, Nop, func.size() - patchBytesSize);
					patch.initPatch(prgm->getCodePtr(func.start + code_offset), fullPatchBytes, func.size());
					delete[] fullPatchBytes;
				}
			}
		}
	}

	void ScriptFuncPatch::onDisable(const hash_t script)
	{
		if (rage::scrProgram::fromHash(script) == nullptr)
		{
			patch.dispose();
		}
		else
		{
			patch.restore();
		}
	}
}
