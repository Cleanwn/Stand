#pragma once

#include <soup/Bytepatch.hpp>

#include "hashtype.hpp"

namespace Stand
{
	class ScriptFuncPatch
	{
	private:
		soup::Bytepatch patch;

	public:
		static uint8_t patchBytesReturnVoid_0[4];
		static uint8_t patchBytesReturnVoid_1[4];

		static uint8_t patchBytesReturnFalse_1[5];
		static uint8_t patchBytesReturnTrue_1[5];
		static uint8_t patchBytesReturnTrue_2[5];
		static uint8_t patchBytesReturnTrue_3[5];

		static uint8_t patchBytesNop3[3];

		void onTick(const hash_t script, const uint32_t func_i, uint8_t* const patchBytes, const size_t patchBytesSize, const uint32_t code_offset = 0);
		void onDisable(const hash_t script);
	};
}
