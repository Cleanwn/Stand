#include "script_vm_reimpl.hpp"

#if SVM_REIMPL

#include <soup/base.hpp>

#include "atStringHash.hpp"
#include "FileLogger.hpp"
#include "Metrics.hpp"
#include "scrOpcodes.hpp"
#include "script_thread.hpp"
#include "ScriptVmErrorHandling.hpp"

#define DEBUG_VFTCORRUPTION false

#if DEBUG_VFTCORRUPTION
#include "atArray.hpp"
#include "pointers.hpp"
#endif

// This adds 5+ minutes to the compile time and saves maybe 0.01 ms per tick...
#define ENABLE_BPOPTIM false

#if !ENABLE_BPOPTIM
// Can't really see a performance difference
#define ENABLE_INSN_PREFETCHING false
#endif

namespace rage
{
	using namespace Stand;

	using s8 = int8_t;
	using s16 = int16_t;
	using s32 = int32_t;
	using s64 = int64_t;

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	constexpr u32 scrPageShift = 14;
	constexpr u32 scrPageSize = (1 << scrPageShift);
	constexpr u32 scrPageMask = scrPageSize - 1;

	constexpr u32 scrStringShift = 14;
	constexpr u32 scrStringSize = (1 << scrStringShift);
	constexpr u32 scrStringMask = (scrStringSize - 1);

	constexpr u32 scrGlobalsPageShift = 14;
	constexpr u32 scrGlobalsPageSize = (1 << scrGlobalsPageShift);
	constexpr u32 scrGlobalsPageMask = (scrGlobalsPageSize - 1);

#define ptrdiff_t_to_int(x)  ((int)(x))

#define Fault(...) \
{ \
	auto* thread = rage::scrThread::get(); \
	thread->m_context.m_instruction_pointer = ptrdiff_t_to_int(pc - opcodes); \
	return ScriptVmErrorHandling::onScriptThreadErrorInternal(reinterpret_cast<uint64_t>(sp), reinterpret_cast<uint64_t>(fp)); \
}

#define LoadImm32	((pc+=4), *(u32*)(pc-3))
#if defined(__SNC__) || defined(_XBOX_VER)
#define LoadImm24	((pc+=3), *(u32*)(pc-3) & 0xFFFFFF)
#else
#define LoadImm24	((pc+=3), *(u32*)(pc-3) >> 8)
#endif
#define LoadImm16	((pc+=2), *(u16*)(pc-1))
#define LoadImmS16	((pc+=2), *(s16*)(pc-1))
#define LoadImm8	(*++pc)

#if false
	// Pick a reasonable value that fits in a PPC compare immediate insn.
#define VALIDATE_PC(target) do { if (target < 4) Fault("Impossible jump target caught"); } while (0)
#define VALIDATE_PTR(value)	do { if (value.Uns < 16384) Fault("Invalid reference caught"); } while (0)
#else
#define VALIDATE_PC(target)
#define VALIDATE_PTR(value)
#endif

#define SET_PC(_o)	do { intptr_t o = _o; pc = (opcodesTbl[o>>scrPageShift] + (o & scrPageMask) - 1); opcodes = pc - o; } while (0)
#define ADD_PC(_r)	SET_PC((pc - opcodes) + (_r))
#define CHECK_PC	SET_PC(pc - opcodes)

#define scrEncodePointer(x)		(x)
#define scrDecodePointer(x)		(x)
#define scrStringLiteral(n,x)	const char *n = x
#define scrEncodeString(x)		(x)
#define scrDecodeString(x)		(x)
#define scrEncodeReference(x)	(x)
#define scrDecodeReference(x)	(x)

#define c_NativeInsnLength 4

	inline float scr_fmodf(float x, float y) { return y ? x - ((int)(x / y) * y) : 0; }

	static void scr_assign_string(char* dst, unsigned siz, const char* src)
	{
		if (src)
		{
			while (*src && --siz)
			{
				*dst++ = *src++;
			}
		}
		*dst = '\0';
	}

	static void scr_append_string(char* dst, unsigned siz, const char* src)
	{
		while (*dst)
		{
			dst++, --siz;
		}
		scr_assign_string(dst, siz, src);
	}

	static void scr_itoa(char* dest, int value)
	{
		char stack[16], * sp = stack;
		if (value < 0)
		{
			*dest++ = '-';
			value = -value;
		}
		else if (!value)
		{
			dest[0] = '0';
			dest[1] = 0;
			return;
		}
		while (value)
		{
			*sp++ = (char)((value % 10) + '0');
			value /= 10;
		}
		while (sp != stack)
		{
			*dest++ = *--sp;
		}
		*dest = 0;
	}

#if DEBUG_VFTCORRUPTION
	static bool areVftsIntact()
	{
		__try
		{
			void* volatile deez;
			for (const auto& thrd : *pointers::script_threads)
			{
				if (thrd)
				{
					deez = **reinterpret_cast<void***>(thrd);
				}
			}
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
		return false;
	}
#endif

	u32 script_vm_reimpl(scrValue* stack, scrValue** globals, const scrProgram& pt, scrThreadContext* ser)
	{
#if DEBUG_VFTCORRUPTION
		if (!areVftsIntact())
		{
			g_logger.log("vfts are not intact, not running scripts.");
			return rage::scrThread::BLOCKED;
		}
#endif

		const u8** opcodesTbl = const_cast<const u8**>(pt.m_code_blocks);
		scrValue* sp = stack + ser->m_stack_pointer - 1;
		scrValue* fp = stack + ser->m_frame_pointer;
		const u8* pc;
		const u8* opcodes;
		char buf[16];
		unsigned insnLimit = 100000000;

		SET_PC(ser->m_instruction_pointer);

#if ENABLE_BPOPTIM
#define CASE(x) l##x:
#define DEFAULT ldefault:
#define FETCH_INSN
#define NEXT_INSN switch (ScriptVmErrorHandling::reimpl_last_opcode = LoadImm8) { case OP_NOP: goto lOP_NOP; case OP_IADD: goto lOP_IADD; case OP_ISUB: goto lOP_ISUB; case OP_IMUL: goto lOP_IMUL; case OP_IDIV: goto lOP_IDIV; case OP_IMOD: goto lOP_IMOD; case OP_INOT: goto lOP_INOT; case OP_INEG: goto lOP_INEG; case OP_IEQ: goto lOP_IEQ; case OP_INE: goto lOP_INE; case OP_IGT: goto lOP_IGT; case OP_IGE: goto lOP_IGE; case OP_ILT: goto lOP_ILT; case OP_ILE: goto lOP_ILE; case OP_FADD: goto lOP_FADD; case OP_FSUB: goto lOP_FSUB; case OP_FMUL: goto lOP_FMUL; case OP_FDIV: goto lOP_FDIV; case OP_FMOD: goto lOP_FMOD; case OP_FNEG: goto lOP_FNEG; case OP_FEQ: goto lOP_FEQ; case OP_FNE: goto lOP_FNE; case OP_FGT: goto lOP_FGT; case OP_FGE: goto lOP_FGE; case OP_FLT: goto lOP_FLT; case OP_FLE: goto lOP_FLE; case OP_VADD: goto lOP_VADD; case OP_VSUB: goto lOP_VSUB; case OP_VMUL: goto lOP_VMUL; case OP_VDIV: goto lOP_VDIV; case OP_VNEG: goto lOP_VNEG; case OP_IAND: goto lOP_IAND; case OP_IOR: goto lOP_IOR; case OP_IXOR: goto lOP_IXOR; case OP_I2F: goto lOP_I2F; case OP_F2I: goto lOP_F2I; case OP_F2V: goto lOP_F2V; case OP_PUSH_CONST_U8: goto lOP_PUSH_CONST_U8; case OP_PUSH_CONST_U8_U8: goto lOP_PUSH_CONST_U8_U8; case OP_PUSH_CONST_U8_U8_U8: goto lOP_PUSH_CONST_U8_U8_U8; case OP_PUSH_CONST_U32: goto lOP_PUSH_CONST_U32; case OP_PUSH_CONST_F: goto lOP_PUSH_CONST_F; case OP_DUP: goto lOP_DUP; case OP_DROP: goto lOP_DROP; case OP_NATIVE: goto lOP_NATIVE; case OP_ENTER: goto lOP_ENTER; case OP_LEAVE: goto lOP_LEAVE; case OP_LOAD: goto lOP_LOAD; case OP_STORE: goto lOP_STORE; case OP_STORE_REV: goto lOP_STORE_REV; case OP_LOAD_N: goto lOP_LOAD_N; case OP_STORE_N: goto lOP_STORE_N; case OP_ARRAY_U8: goto lOP_ARRAY_U8; case OP_ARRAY_U8_LOAD: goto lOP_ARRAY_U8_LOAD; case OP_ARRAY_U8_STORE: goto lOP_ARRAY_U8_STORE; case OP_LOCAL_U8: goto lOP_LOCAL_U8; case OP_LOCAL_U8_LOAD: goto lOP_LOCAL_U8_LOAD; case OP_LOCAL_U8_STORE: goto lOP_LOCAL_U8_STORE; case OP_STATIC_U8: goto lOP_STATIC_U8; case OP_STATIC_U8_LOAD: goto lOP_STATIC_U8_LOAD; case OP_STATIC_U8_STORE: goto lOP_STATIC_U8_STORE; case OP_IADD_U8: goto lOP_IADD_U8; case OP_IMUL_U8: goto lOP_IMUL_U8; case OP_IOFFSET: goto lOP_IOFFSET; case OP_IOFFSET_U8: goto lOP_IOFFSET_U8; case OP_IOFFSET_U8_LOAD: goto lOP_IOFFSET_U8_LOAD; case OP_IOFFSET_U8_STORE: goto lOP_IOFFSET_U8_STORE; case OP_PUSH_CONST_S16: goto lOP_PUSH_CONST_S16; case OP_IADD_S16: goto lOP_IADD_S16; case OP_IMUL_S16: goto lOP_IMUL_S16; case OP_IOFFSET_S16: goto lOP_IOFFSET_S16; case OP_IOFFSET_S16_LOAD: goto lOP_IOFFSET_S16_LOAD; case OP_IOFFSET_S16_STORE: goto lOP_IOFFSET_S16_STORE; case OP_ARRAY_U16: goto lOP_ARRAY_U16; case OP_ARRAY_U16_LOAD: goto lOP_ARRAY_U16_LOAD; case OP_ARRAY_U16_STORE: goto lOP_ARRAY_U16_STORE; case OP_LOCAL_U16: goto lOP_LOCAL_U16; case OP_LOCAL_U16_LOAD: goto lOP_LOCAL_U16_LOAD; case OP_LOCAL_U16_STORE: goto lOP_LOCAL_U16_STORE; case OP_STATIC_U16: goto lOP_STATIC_U16; case OP_STATIC_U16_LOAD: goto lOP_STATIC_U16_LOAD; case OP_STATIC_U16_STORE: goto lOP_STATIC_U16_STORE; case OP_GLOBAL_U16: goto lOP_GLOBAL_U16; case OP_GLOBAL_U16_LOAD: goto lOP_GLOBAL_U16_LOAD; case OP_GLOBAL_U16_STORE: goto lOP_GLOBAL_U16_STORE; case OP_J: goto lOP_J; case OP_JZ: goto lOP_JZ; case OP_IEQ_JZ: goto lOP_IEQ_JZ; case OP_INE_JZ: goto lOP_INE_JZ; case OP_IGT_JZ: goto lOP_IGT_JZ; case OP_IGE_JZ: goto lOP_IGE_JZ; case OP_ILT_JZ: goto lOP_ILT_JZ; case OP_ILE_JZ: goto lOP_ILE_JZ; case OP_CALL: goto lOP_CALL; case OP_GLOBAL_U24: goto lOP_GLOBAL_U24; case OP_GLOBAL_U24_LOAD: goto lOP_GLOBAL_U24_LOAD; case OP_GLOBAL_U24_STORE: goto lOP_GLOBAL_U24_STORE; case OP_PUSH_CONST_U24: goto lOP_PUSH_CONST_U24; case OP_SWITCH: goto lOP_SWITCH; case OP_STRING: goto lOP_STRING; case OP_STRINGHASH: goto lOP_STRINGHASH; case OP_TEXT_LABEL_ASSIGN_STRING: goto lOP_TEXT_LABEL_ASSIGN_STRING; case OP_TEXT_LABEL_ASSIGN_INT: goto lOP_TEXT_LABEL_ASSIGN_INT; case OP_TEXT_LABEL_APPEND_STRING: goto lOP_TEXT_LABEL_APPEND_STRING; case OP_TEXT_LABEL_APPEND_INT: goto lOP_TEXT_LABEL_APPEND_INT; case OP_TEXT_LABEL_COPY: goto lOP_TEXT_LABEL_COPY; case OP_CATCH: goto lOP_CATCH; case OP_THROW: goto lOP_THROW; case OP_CALLINDIRECT: goto lOP_CALLINDIRECT; case OP_PUSH_CONST_M1: goto lOP_PUSH_CONST_M1; case OP_PUSH_CONST_0: goto lOP_PUSH_CONST_0; case OP_PUSH_CONST_1: goto lOP_PUSH_CONST_1; case OP_PUSH_CONST_2: goto lOP_PUSH_CONST_2; case OP_PUSH_CONST_3: goto lOP_PUSH_CONST_3; case OP_PUSH_CONST_4: goto lOP_PUSH_CONST_4; case OP_PUSH_CONST_5: goto lOP_PUSH_CONST_5; case OP_PUSH_CONST_6: goto lOP_PUSH_CONST_6; case OP_PUSH_CONST_7: goto lOP_PUSH_CONST_7; case OP_PUSH_CONST_FM1: goto lOP_PUSH_CONST_FM1; case OP_PUSH_CONST_F0: goto lOP_PUSH_CONST_F0; case OP_PUSH_CONST_F1: goto lOP_PUSH_CONST_F1; case OP_PUSH_CONST_F2: goto lOP_PUSH_CONST_F2; case OP_PUSH_CONST_F3: goto lOP_PUSH_CONST_F3; case OP_PUSH_CONST_F4: goto lOP_PUSH_CONST_F4; case OP_PUSH_CONST_F5: goto lOP_PUSH_CONST_F5; case OP_PUSH_CONST_F6: goto lOP_PUSH_CONST_F6; case OP_PUSH_CONST_F7: goto lOP_PUSH_CONST_F7; case OP_IS_BIT_SET: goto lOP_IS_BIT_SET; }
#else
#define CASE(x) case x:
#define DEFAULT default:
#if ENABLE_INSN_PREFETCHING
#define FETCH_INSN next_insn = LoadImm8
#else
#define FETCH_INSN
#endif
#define NEXT_INSN goto DISPATCH

#if ENABLE_INSN_PREFETCHING
		auto next_insn = LoadImm8;
#endif
	DISPATCH:
#endif
		if (!--insnLimit)
		{
			return 69;
		}

#if DEBUG_VFTCORRUPTION
		if (!areVftsIntact())
		{
			g_logger.log(fmt::format("Corruption after {}", insn));
			return rage::scrThread::RUNNING;
		}
#endif
#if !ENABLE_BPOPTIM
#if ENABLE_INSN_PREFETCHING
		ScriptVmErrorHandling::reimpl_last_opcode = next_insn;
		switch (next_insn)
#else
		switch (ScriptVmErrorHandling::reimpl_last_opcode = LoadImm8)
#endif
#endif
		{
			CASE(OP_NOP) CHECK_PC; FETCH_INSN; NEXT_INSN;
			CASE(OP_IADD) FETCH_INSN; --sp; sp[0].Int += sp[1].Int; NEXT_INSN;
			CASE(OP_ISUB) FETCH_INSN; --sp; sp[0].Int -= sp[1].Int; NEXT_INSN;
			CASE(OP_IMUL) FETCH_INSN; --sp; sp[0].Int *= sp[1].Int; NEXT_INSN;
			CASE(OP_IDIV) FETCH_INSN; --sp; if (sp[1].Int) sp[0].Int /= sp[1].Int; NEXT_INSN;
			CASE(OP_IMOD) FETCH_INSN; --sp; if (sp[1].Int) sp[0].Int %= sp[1].Int; NEXT_INSN;
			CASE(OP_INOT) FETCH_INSN; sp[0].Int = !sp[0].Int; NEXT_INSN;
			CASE(OP_INEG) FETCH_INSN; sp[0].Int = -sp[0].Int; NEXT_INSN;

			CASE(OP_IEQ) FETCH_INSN; --sp; sp[0].Int = sp[0].Int == sp[1].Int; NEXT_INSN;
			CASE(OP_INE) FETCH_INSN; --sp; sp[0].Int = sp[0].Int != sp[1].Int; NEXT_INSN;
			CASE(OP_IGE) FETCH_INSN; --sp; sp[0].Int = sp[0].Int >= sp[1].Int; NEXT_INSN;
			CASE(OP_IGT) FETCH_INSN; --sp; sp[0].Int = sp[0].Int > sp[1].Int; NEXT_INSN;
			CASE(OP_ILE) FETCH_INSN; --sp; sp[0].Int = sp[0].Int <= sp[1].Int; NEXT_INSN;
			CASE(OP_ILT) FETCH_INSN; --sp; sp[0].Int = sp[0].Int < sp[1].Int; NEXT_INSN;

			CASE(OP_FADD) FETCH_INSN; --sp; sp[0].Float += sp[1].Float; NEXT_INSN;
			CASE(OP_FSUB) FETCH_INSN; --sp; sp[0].Float -= sp[1].Float; NEXT_INSN;
			CASE(OP_FMUL) FETCH_INSN; --sp; sp[0].Float *= sp[1].Float; NEXT_INSN;
			CASE(OP_FDIV) FETCH_INSN; --sp; if (sp[1].Int) sp[0].Float /= sp[1].Float; NEXT_INSN;
			CASE(OP_FMOD) FETCH_INSN; --sp; if (sp[1].Int) sp[0].Float = scr_fmodf(sp[0].Float, sp[1].Float); NEXT_INSN;
			CASE(OP_FNEG) FETCH_INSN; sp[0].Uns ^= 0x80000000; NEXT_INSN;

			CASE(OP_FEQ) FETCH_INSN; --sp; sp[0].Int = sp[0].Float == sp[1].Float; NEXT_INSN;	// TODO: Use integer compare?
			CASE(OP_FNE) FETCH_INSN; --sp; sp[0].Int = sp[0].Float != sp[1].Float; NEXT_INSN;	// TODO: Use integer compare?
			CASE(OP_FGE) FETCH_INSN; --sp; sp[0].Int = sp[0].Float >= sp[1].Float; NEXT_INSN;
			CASE(OP_FGT) FETCH_INSN; --sp; sp[0].Int = sp[0].Float > sp[1].Float; NEXT_INSN;
			CASE(OP_FLE) FETCH_INSN; --sp; sp[0].Int = sp[0].Float <= sp[1].Float; NEXT_INSN;
			CASE(OP_FLT) FETCH_INSN; --sp; sp[0].Int = sp[0].Float < sp[1].Float; NEXT_INSN;

			CASE(OP_VADD) FETCH_INSN; sp -= 3; sp[-2].Float += sp[1].Float; sp[-1].Float += sp[2].Float; sp[0].Float += sp[3].Float; NEXT_INSN;
			CASE(OP_VSUB) FETCH_INSN; sp -= 3; sp[-2].Float -= sp[1].Float; sp[-1].Float -= sp[2].Float; sp[0].Float -= sp[3].Float; NEXT_INSN;
			CASE(OP_VMUL) FETCH_INSN; sp -= 3; sp[-2].Float *= sp[1].Float; sp[-1].Float *= sp[2].Float; sp[0].Float *= sp[3].Float; NEXT_INSN;
			CASE(OP_VDIV) FETCH_INSN; sp -= 3; if (sp[1].Int) sp[-2].Float /= sp[1].Float; if (sp[2].Int) sp[-1].Float /= sp[2].Float; if (sp[3].Int) sp[0].Float /= sp[3].Float; NEXT_INSN;
			CASE(OP_VNEG) FETCH_INSN; sp[-2].Uns ^= 0x80000000; sp[-1].Uns ^= 0x80000000; sp[0].Uns ^= 0x80000000; NEXT_INSN;

			CASE(OP_IAND) FETCH_INSN; --sp; sp[0].Int &= sp[1].Int; NEXT_INSN;
			CASE(OP_IOR)  FETCH_INSN; --sp; sp[0].Int |= sp[1].Int; NEXT_INSN;
			CASE(OP_IXOR) FETCH_INSN; --sp; sp[0].Int ^= sp[1].Int; NEXT_INSN;

			CASE(OP_I2F) FETCH_INSN; sp[0].Float = (float)sp[0].Int; NEXT_INSN;
			CASE(OP_F2I) FETCH_INSN; sp[0].Int = (int)sp[0].Float; NEXT_INSN;
			CASE(OP_F2V) FETCH_INSN; sp += 2; sp[-1].Int = sp[0].Int = sp[-2].Int; NEXT_INSN;

			CASE(OP_PUSH_CONST_U8) ++sp; sp[0].Int = LoadImm8; FETCH_INSN; NEXT_INSN;
			CASE(OP_PUSH_CONST_U8_U8) sp += 2; sp[-1].Int = LoadImm8; sp[0].Int = LoadImm8; FETCH_INSN; NEXT_INSN;
			CASE(OP_PUSH_CONST_U8_U8_U8) sp += 3; sp[-2].Int = LoadImm8; sp[-1].Int = LoadImm8; sp[0].Int = LoadImm8; FETCH_INSN; NEXT_INSN;

			CASE(OP_PUSH_CONST_U32)
				CASE(OP_PUSH_CONST_F) ++sp; sp[0].Uns = LoadImm32; FETCH_INSN; NEXT_INSN;

			CASE(OP_DUP) FETCH_INSN; ++sp; sp[0].Any = sp[-1].Any; NEXT_INSN;
			CASE(OP_DROP) FETCH_INSN; --sp; NEXT_INSN;

			CASE(OP_NATIVE)
			{
				int returnSize = LoadImm8;
				int paramCount = (returnSize >> 2) & 63;
				u32 imm = (LoadImm8 << 8);
				imm |= LoadImm8;
				returnSize &= 3;
				scrNativeHandler cmd = (scrNativeHandler)(size_t)pt.m_native_entrypoints[imm];

				++Metrics::natives_game_invoked_this_tick;
				ScriptVmErrorHandling::reimpl_last_native_entrypoint = cmd;

				// TrapGE(imm,pt.NativeSize);
				// Preserve state so it's correct if the function blocked, and more importantly, in case
				// the function does anything nasty like subclass our thread class and access these directly.
				ser->m_instruction_pointer = ptrdiff_t_to_int(pc - opcodes - c_NativeInsnLength);
				ser->m_frame_pointer = ptrdiff_t_to_int(fp - stack);
				ser->m_stack_pointer = ptrdiff_t_to_int(sp - stack + 1);
				scrNativeCallContext curInfo(returnSize ? &stack[ser->m_stack_pointer - paramCount] : 0, paramCount, &stack[ser->m_stack_pointer - paramCount]);
				(*cmd)(curInfo);
				if (ser->m_state != scrThread::RUNNING)
				{
					return ser->m_state;
				}
				// If we didn't block, copy referenced parameters back into caller's stack.
				curInfo.CopyReferencedParametersOut();
				// Drop call parameters off of the stack but leave any return result there.
				sp -= paramCount - returnSize;
				FETCH_INSN;
				NEXT_INSN;
			}

			CASE(OP_ENTER)
			{
				u32 paramCount = LoadImm8;
				u32 localCount = LoadImm16;
				u32 nameCount = LoadImm8;

				/*if (ser->m_CallDepth < MAX_CALLSTACK)
				{
					ser->m_CallStack[ser->m_CallDepth] = ptrdiff_t_to_int(pc - opcodes + 2);
				}*/
				++(ser->m_CallDepth);

				pc += nameCount;
				FETCH_INSN;
				SOUP_IF_UNLIKELY (sp - stack >= (int)(ser->m_stack_size - localCount))
				{
					Fault("Stack overflow");
				}
				(++sp)->Int = ptrdiff_t_to_int(fp - stack);
				fp = sp - paramCount - 1;

#if false
				memset((void*)&sp[1].Any, 0, localCount * sizeof(scrValue));
				sp += localCount;
#else
				while (localCount--)
				{
					(++sp)->Any = 0;
				}
#endif
				sp -= paramCount;
				NEXT_INSN;
			}

			CASE(OP_LEAVE)
			{
				--(ser->m_CallDepth);
				u32 paramCount = LoadImm8;	// Get number of parameters to clean off the stack
				u32 returnSize = LoadImm8;	// Get size of the return value
				scrValue* result = sp - returnSize;	// Remember start of return value
				sp = fp + paramCount - 1;
				fp = stack + sp[2].Uns;
				u32 newPc = sp[1].Uns;		// Pull return address off of the stack placed by OP_CALL/OP_CALLINDIRECT;
				SET_PC(newPc);
				sp -= paramCount;				// Drop the caller's parameters
				// Copy the return value to the new top-of-stack
				while (returnSize--)
				{
					*++sp = *++result;
				}
				if (!newPc)
				{
					return ser->m_state = scrThread::ABORTED;
				}
				FETCH_INSN;
				NEXT_INSN;
			}

			CASE(OP_LOAD) FETCH_INSN; VALIDATE_PTR(sp[0]); sp[0].Any = scrDecodeReference(sp[0].Reference)->Any; NEXT_INSN;
			CASE(OP_STORE) FETCH_INSN; sp -= 2; VALIDATE_PTR(sp[2]); scrDecodeReference(sp[2].Reference)->Any = sp[1].Any; NEXT_INSN;
			CASE(OP_STORE_REV) FETCH_INSN; --sp; VALIDATE_PTR(sp[0]); scrDecodeReference(sp[0].Reference)->Any = sp[1].Any; NEXT_INSN;

			CASE(OP_LOAD_N)
			{
				FETCH_INSN;
				scrValue* addr = scrDecodeReference((sp--)->Reference);
				u32 count = (sp--)->Int;
				for (u32 i = 0; i < count; i++)
				{
					(++sp)->Any = addr[i].Any;
				}
				NEXT_INSN;
			}

			CASE(OP_STORE_N)
			{
				FETCH_INSN;
				// Values are pushed up to the stack in evaluation order,
				// so reverse them when storing
				scrValue* addr = scrDecodeReference((sp--)->Reference);
				u32 count = (sp--)->Int;
				for (u32 i = 0; i < count; i++)
				{
					addr[count - 1 - i].Any = (sp--)->Any;
				}
				NEXT_INSN;
			}

			CASE(OP_ARRAY_U8)
			{
				--sp;
				scrValue* r = scrDecodeReference(sp[1].Reference);
				u32 idx = sp[0].Uns;
				SOUP_IF_UNLIKELY (idx >= r->Uns)
				{
					Fault("Array overrun, %u >= %u", idx, r->Uns);
				}
				r += 1U + idx * LoadImm8;
				FETCH_INSN;
				sp[0].Reference = scrEncodeReference(r);
				NEXT_INSN;
			}

			CASE(OP_ARRAY_U8_LOAD)
			{
				--sp;
				scrValue* r = scrDecodeReference(sp[1].Reference);
				u32 idx = sp[0].Uns;
				SOUP_IF_UNLIKELY (idx >= r->Uns)
				{
					Fault("Array overrun, %u >= %u", idx, r->Uns);
				}
				r += 1U + idx * LoadImm8;
				FETCH_INSN;
				sp[0].Any = r->Any;
				NEXT_INSN;
			}

			CASE(OP_ARRAY_U8_STORE)
			{
				sp -= 3;
				scrValue* r = scrDecodeReference(sp[3].Reference);
				u32 idx = sp[2].Uns;
				SOUP_IF_UNLIKELY (idx >= r->Uns)
				{
					Fault("Array overrun, %u >= %u", idx, r->Uns);
				}
				r += 1U + idx * LoadImm8;
				FETCH_INSN;
				r->Any = sp[1].Any;
				NEXT_INSN;
			}

			CASE(OP_LOCAL_U8) ++sp; sp[0].Reference = scrEncodeReference(fp + LoadImm8); FETCH_INSN; NEXT_INSN;
			CASE(OP_LOCAL_U8_LOAD) ++sp; sp[0].Any = fp[LoadImm8].Any; FETCH_INSN; NEXT_INSN;
			CASE(OP_LOCAL_U8_STORE) --sp; fp[LoadImm8].Any = sp[1].Any; FETCH_INSN; NEXT_INSN;

			CASE(OP_STATIC_U8) ++sp; sp[0].Reference = scrEncodeReference(stack + LoadImm8); FETCH_INSN; NEXT_INSN;
			CASE(OP_STATIC_U8_LOAD) ++sp; sp[0].Any = stack[LoadImm8].Any; FETCH_INSN; NEXT_INSN;
			CASE(OP_STATIC_U8_STORE) --sp; stack[LoadImm8].Any = sp[1].Any; FETCH_INSN; NEXT_INSN;

			CASE(OP_IADD_U8) sp[0].Int += LoadImm8; FETCH_INSN; NEXT_INSN;
			CASE(OP_IMUL_U8) sp[0].Int *= LoadImm8; FETCH_INSN; NEXT_INSN;

			CASE(OP_IOFFSET) FETCH_INSN; --sp; sp[0].Any += sp[1].Int * sizeof(scrValue); NEXT_INSN;
			CASE(OP_IOFFSET_U8) sp[0].Any += LoadImm8 * sizeof(scrValue); FETCH_INSN; NEXT_INSN;
			CASE(OP_IOFFSET_U8_LOAD) VALIDATE_PTR(sp[0]); sp[0].Any = scrDecodeReference(sp[0].Reference)[LoadImm8].Any; FETCH_INSN; NEXT_INSN;
			CASE(OP_IOFFSET_U8_STORE) sp -= 2; VALIDATE_PTR(sp[2]); scrDecodeReference(sp[2].Reference)[LoadImm8].Any = sp[1].Any; FETCH_INSN; NEXT_INSN;

			CASE(OP_PUSH_CONST_S16) ++sp; sp[0].Int = LoadImmS16; FETCH_INSN; NEXT_INSN;
			CASE(OP_IADD_S16) sp[0].Int += LoadImmS16; FETCH_INSN; NEXT_INSN;
			CASE(OP_IMUL_S16) sp[0].Int *= LoadImmS16; FETCH_INSN; NEXT_INSN;

			CASE(OP_IOFFSET_S16) sp[0].Any += LoadImmS16 * sizeof(scrValue); FETCH_INSN; NEXT_INSN;
			CASE(OP_IOFFSET_S16_LOAD) VALIDATE_PTR(sp[0]); sp[0].Any = scrDecodeReference(sp[0].Reference)[LoadImmS16].Any; FETCH_INSN; NEXT_INSN;
			CASE(OP_IOFFSET_S16_STORE) sp -= 2; VALIDATE_PTR(sp[2]); scrDecodeReference(sp[2].Reference)[LoadImmS16].Any = sp[1].Any; FETCH_INSN; NEXT_INSN;

			CASE(OP_ARRAY_U16)
			{
				--sp;
				scrValue* r = scrDecodeReference(sp[1].Reference);
				u32 idx = sp[0].Uns;
				SOUP_IF_UNLIKELY (idx >= r->Uns)
				{
					Fault("Array overrun, %u >= %u", idx, r->Uns);
				}
				r += 1U + idx * LoadImm16;
				FETCH_INSN;
				sp[0].Reference = scrEncodeReference(r);
				NEXT_INSN;
			}

			CASE(OP_ARRAY_U16_LOAD)
			{
				--sp;
				scrValue* r = scrDecodeReference(sp[1].Reference);
				u32 idx = sp[0].Uns;
				SOUP_IF_UNLIKELY (idx >= r->Uns)
				{
					Fault("Array overrun, %u >= %u", idx, r->Uns);
				}
				r += 1U + idx * LoadImm16;
				FETCH_INSN;
				sp[0].Any = r->Any;
				NEXT_INSN;
			}

			CASE(OP_ARRAY_U16_STORE)
			{
				sp -= 3;
				scrValue* r = scrDecodeReference(sp[3].Reference);
				u32 idx = sp[2].Uns;
				SOUP_IF_UNLIKELY (idx >= r->Uns)
				{
					Fault("Array overrun, %u >= %u", idx, r->Uns);
				}
				r += 1U + idx * LoadImm16;
				FETCH_INSN;
				r->Any = sp[1].Any;
				NEXT_INSN;
			}

			CASE(OP_LOCAL_U16) ++sp; sp[0].Reference = scrEncodeReference(fp + LoadImm16); FETCH_INSN; NEXT_INSN;
			CASE(OP_LOCAL_U16_LOAD) ++sp; sp[0].Any = fp[LoadImm16].Any; FETCH_INSN; NEXT_INSN;
			CASE(OP_LOCAL_U16_STORE) --sp; fp[LoadImm16].Any = sp[1].Any; FETCH_INSN; NEXT_INSN;

			CASE(OP_STATIC_U16) ++sp; sp[0].Reference = scrEncodeReference(stack + LoadImm16); FETCH_INSN; NEXT_INSN;
			CASE(OP_STATIC_U16_LOAD) ++sp; sp[0].Any = stack[LoadImm16].Any; FETCH_INSN; NEXT_INSN;
			CASE(OP_STATIC_U16_STORE) --sp; stack[LoadImm16].Any = sp[1].Any; FETCH_INSN; NEXT_INSN;

			CASE(OP_GLOBAL_U16) ++sp; sp[0].Reference = scrEncodeReference(globals[0] + LoadImm16); FETCH_INSN; NEXT_INSN;
			CASE(OP_GLOBAL_U16_LOAD) ++sp; sp[0].Any = globals[0][LoadImm16].Any; FETCH_INSN; NEXT_INSN;
			CASE(OP_GLOBAL_U16_STORE) --sp; globals[0][LoadImm16].Any = sp[1].Any; FETCH_INSN; NEXT_INSN;

			CASE(OP_STATIC_U24) ++sp; sp[0].Reference = scrEncodeReference(stack + LoadImm24); FETCH_INSN; NEXT_INSN;
			CASE(OP_STATIC_U24_LOAD) ++sp; sp[0].Any = stack[LoadImm24].Any; FETCH_INSN; NEXT_INSN;
			CASE(OP_STATIC_U24_STORE) --sp; stack[LoadImm24].Any = sp[1].Any; FETCH_INSN; NEXT_INSN;

			CASE(OP_GLOBAL_U24)
			{
				u32 imm = LoadImm24, block = imm >> scrProgram::MAX_GLOBAL_BLOCKS_SHIFT, ofs = imm & scrProgram::GLOBAL_SIZE_MASK;
				++sp;
				/*SOUP_IF_UNLIKELY (!globals[block])
				{
					Fault("Global block %u/%u not resident", block, ofs);
				}
				else*/
				{
					sp[0].Reference = scrEncodeReference(&globals[block][ofs]);
				}
				FETCH_INSN; NEXT_INSN;
			}
			CASE(OP_GLOBAL_U24_LOAD)
			{
				u32 imm = LoadImm24, block = imm >> scrProgram::MAX_GLOBAL_BLOCKS_SHIFT, ofs = imm & scrProgram::GLOBAL_SIZE_MASK;
				++sp;
				/*SOUP_IF_UNLIKELY (!globals[block])
				{
					Fault("Global block %u/%u not resident", block, ofs);
				}
				else*/
				{
					sp[0].Any = globals[block][ofs].Any;
				}
				FETCH_INSN; NEXT_INSN;
			}
			CASE(OP_GLOBAL_U24_STORE)
			{
				u32 imm = LoadImm24, block = imm >> scrProgram::MAX_GLOBAL_BLOCKS_SHIFT, ofs = imm & scrProgram::GLOBAL_SIZE_MASK;
				--sp;
				/*SOUP_IF_UNLIKELY (!globals[block])
				{
					Fault("Global block %u/%u not resident", block, ofs);
				}
				else*/
				{
					globals[block][ofs].Any = sp[1].Any;
				}
				FETCH_INSN; NEXT_INSN;
			}
			CASE(OP_PUSH_CONST_U24) ++sp; sp[0].Int = LoadImm24; FETCH_INSN; NEXT_INSN;

			CASE(OP_CALL) { u32 imm = LoadImm24; ++sp; sp[0].Uns = ptrdiff_t_to_int(pc - opcodes); SET_PC(imm); FETCH_INSN; NEXT_INSN; }

			CASE(OP_J) { s32 imm = LoadImmS16; ADD_PC(imm); FETCH_INSN; NEXT_INSN; }
			CASE(OP_JZ) { s32 imm = LoadImmS16; --sp; if (sp[1].Int == 0) ADD_PC(imm); else CHECK_PC; FETCH_INSN; NEXT_INSN; }
			CASE(OP_IEQ_JZ) { s32 imm = LoadImmS16; sp -= 2; if (!(sp[1].Int == sp[2].Int)) ADD_PC(imm); else CHECK_PC; FETCH_INSN; NEXT_INSN; }
			CASE(OP_INE_JZ) { s32 imm = LoadImmS16; sp -= 2; if (!(sp[1].Int != sp[2].Int)) ADD_PC(imm); else CHECK_PC; FETCH_INSN; NEXT_INSN; }
			CASE(OP_IGE_JZ) { s32 imm = LoadImmS16; sp -= 2; if (!(sp[1].Int >= sp[2].Int)) ADD_PC(imm); else CHECK_PC; FETCH_INSN; NEXT_INSN; }
			CASE(OP_IGT_JZ) { s32 imm = LoadImmS16; sp -= 2; if (!(sp[1].Int > sp[2].Int)) ADD_PC(imm); else CHECK_PC; FETCH_INSN; NEXT_INSN; }
			CASE(OP_ILE_JZ) { s32 imm = LoadImmS16; sp -= 2; if (!(sp[1].Int <= sp[2].Int)) ADD_PC(imm); else CHECK_PC; FETCH_INSN; NEXT_INSN; }
			CASE(OP_ILT_JZ) { s32 imm = LoadImmS16; sp -= 2; if (!(sp[1].Int < sp[2].Int)) ADD_PC(imm); else CHECK_PC; FETCH_INSN; NEXT_INSN; }

			CASE(OP_SWITCH)
			{
				--sp;
				u32 label = sp[1].Uns;
				u32 count = LoadImm8;
				CHECK_PC;
				for (u32 i = 0; i < count; i++) {
					u32 match = LoadImm32;
					u32 target = LoadImm16;
					if (label == match) {
						ADD_PC(target);
						break;
					}
				}
				CHECK_PC;
				FETCH_INSN;
				NEXT_INSN;
			}
			CASE(OP_STRING)
			{
				u32 offset = sp[0].Uns;
				sp[0].String = scrEncodeString(pt.m_strings_data[offset >> scrStringShift] + (offset & scrStringMask));
				FETCH_INSN;
				NEXT_INSN;
			}

			CASE(OP_TEXT_LABEL_ASSIGN_STRING)
			{
				sp -= 2;
				char* dest = const_cast<char*>(scrDecodeString(sp[2].String));
				const char* src = scrDecodeString(sp[1].String);
				scr_assign_string(dest, LoadImm8, src);
				FETCH_INSN;
				NEXT_INSN;
			}

			CASE(OP_TEXT_LABEL_ASSIGN_INT)
			{
				sp -= 2;
				char* dest = const_cast<char*>(scrDecodeString(sp[2].String));
				int value = sp[1].Int;
				scr_itoa(buf, value);
				scr_assign_string(dest, LoadImm8, buf);
				FETCH_INSN;
				NEXT_INSN;
			}
			CASE(OP_TEXT_LABEL_APPEND_STRING)
			{
				sp -= 2;
				char* dest = const_cast<char*>(scrDecodeString(sp[2].String));
				const char* src = scrDecodeString(sp[1].String);
				scr_append_string(dest, LoadImm8, src);
				FETCH_INSN;
				NEXT_INSN;
			}
			CASE(OP_TEXT_LABEL_APPEND_INT)
			{
				sp -= 2;
				char* dest = const_cast<char*>(scrDecodeString(sp[2].String));
				int value = sp[1].Int;
				scr_itoa(buf, value);
				scr_append_string(dest, LoadImm8, buf);
				FETCH_INSN;
				NEXT_INSN;
			}

			CASE(OP_TEXT_LABEL_COPY)
			{
				FETCH_INSN;
				sp -= 3;
				scrValue* dest = scrDecodeReference(sp[3].Reference);
				int destSize = sp[2].Int;
				int srcSize = sp[1].Int;
				// Remove excess
				while (srcSize > destSize) {
					--srcSize;
					--sp;
				}
				// Do the bulk of the copy
				for (int i = 0; i < srcSize; i++)
				{
					dest[srcSize - 1 - i].Any = (sp--)->Any;
				}
				// Make sure it's still NUL-terminated
				char* cDest = (char*)dest;
				cDest[(srcSize * sizeof(scrValue)) - 1] = '\0';
				NEXT_INSN;
			}

			CASE(OP_CATCH)
				FETCH_INSN;
			ser->m_CatchPC = ptrdiff_t_to_int(pc - opcodes);
			ser->m_CatchFP = ptrdiff_t_to_int(fp - stack);
			ser->m_CatchSP = ptrdiff_t_to_int(sp - stack + 1);
			++sp;
			sp[0].Int = -1;
			NEXT_INSN;

			CASE(OP_THROW)
			{
				int imm = sp[0].Int;	// caught value (sp is now wrong but will be replaced below)
				SOUP_IF_UNLIKELY (!ser->m_CatchPC)
				{
					Fault("THROW with no CATCH");
				}
				else
				{
					SET_PC(ser->m_CatchPC);
					fp = stack + ser->m_CatchFP;
					sp = stack + ser->m_CatchSP;
					sp[0].Int = imm;
				}
				FETCH_INSN;
				NEXT_INSN;
			}

			CASE(OP_CALLINDIRECT)
			{
				u32 imm = sp[0].Uns;
				SOUP_IF_UNLIKELY (!imm)
				{
					Fault("Attempted to call through uninitialized (zero) function pointer");
				}
				sp[0].Uns = ptrdiff_t_to_int(pc - opcodes);
				SET_PC(imm);
				FETCH_INSN;
				NEXT_INSN;
			}

			CASE(OP_PUSH_CONST_M1) FETCH_INSN; ++sp; sp[0].Int = -1; NEXT_INSN;
			CASE(OP_PUSH_CONST_0) FETCH_INSN; ++sp; sp[0].Any = 0; NEXT_INSN;
			CASE(OP_PUSH_CONST_1) FETCH_INSN; ++sp; sp[0].Int = 1; NEXT_INSN;
			CASE(OP_PUSH_CONST_2) FETCH_INSN; ++sp; sp[0].Int = 2; NEXT_INSN;
			CASE(OP_PUSH_CONST_3) FETCH_INSN; ++sp; sp[0].Int = 3; NEXT_INSN;
			CASE(OP_PUSH_CONST_4) FETCH_INSN; ++sp; sp[0].Int = 4; NEXT_INSN;
			CASE(OP_PUSH_CONST_5) FETCH_INSN; ++sp; sp[0].Int = 5; NEXT_INSN;
			CASE(OP_PUSH_CONST_6) FETCH_INSN; ++sp; sp[0].Int = 6; NEXT_INSN;
			CASE(OP_PUSH_CONST_7) FETCH_INSN; ++sp; sp[0].Int = 7; NEXT_INSN;

			CASE(OP_PUSH_CONST_FM1) FETCH_INSN; ++sp; sp[0].Uns = 0xbf800000; NEXT_INSN;
			CASE(OP_PUSH_CONST_F0) FETCH_INSN; ++sp; sp[0].Uns = 0x00000000; NEXT_INSN;
			CASE(OP_PUSH_CONST_F1) FETCH_INSN; ++sp; sp[0].Uns = 0x3f800000; NEXT_INSN;
			CASE(OP_PUSH_CONST_F2) FETCH_INSN; ++sp; sp[0].Uns = 0x40000000; NEXT_INSN;
			CASE(OP_PUSH_CONST_F3) FETCH_INSN; ++sp; sp[0].Uns = 0x40400000; NEXT_INSN;
			CASE(OP_PUSH_CONST_F4) FETCH_INSN; ++sp; sp[0].Uns = 0x40800000; NEXT_INSN;
			CASE(OP_PUSH_CONST_F5) FETCH_INSN; ++sp; sp[0].Uns = 0x40a00000; NEXT_INSN;
			CASE(OP_PUSH_CONST_F6) FETCH_INSN; ++sp; sp[0].Uns = 0x40c00000; NEXT_INSN;
			CASE(OP_PUSH_CONST_F7) FETCH_INSN; ++sp; sp[0].Uns = 0x40e00000; NEXT_INSN;

			CASE(OP_STRINGHASH) FETCH_INSN; sp[0].Uns = atStringHash(scrDecodeString(sp[0].String)); NEXT_INSN;

			CASE(OP_IS_BIT_SET)
			{
				FETCH_INSN;
				--sp;
				//sp[0].Int = ((sp[0].Int & (1 << sp[1].Int)) != 0);
				sp[0].Int = ((sp[0].Int >> sp[1].Int) & 1);
				// This very simple optimisation saves about 0.02 ms per tick
				NEXT_INSN;
			}

			DEFAULT
				ScriptVmErrorHandling::onInvalidScriptOpCode(ScriptVmErrorHandling::reimpl_last_opcode);
				return scrThread::ABORTED;
		}
		return scrThread::ABORTED;
	}
}

#endif
