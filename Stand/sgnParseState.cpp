#include "sgnParseState.hpp"

#include "script_vm_common.hpp"

namespace ScriptGenie
{
	using namespace Stand;

	// THIS NAMESPACE IS (MORE OR LESS) A MIRROR OF https://github.com/calamity-inc/ScriptGenie

	const uint8_t* ParseState::getCodePtr() const noexcept
	{
		return scr->getCodePtr(code_i);
	}

#define U24_ARG(off) (*(uint32_t*)(code_ptr + off) & 0b11111111'11111111'11111111)

	bool ParseState::tick()
	{
		const uint8_t* code_ptr = getCodePtr();
		if (code_ptr == nullptr)
		{
			return false;
		}
		SOUP_ASSERT(*code_ptr < OpcodesSize);
		switch (*code_ptr)
		{
		case Enter:
			while (!stack.empty()) stack.pop(); // prevent exponential stack growth since we are missing a lot of ops
			break;

		case Return:
			// <byte unk> <byte return_size> - return_size is 0 for void, 1 for int
			break;

		case iPush_n1: stack.push(-1ll); break;
		case iPush_0: stack.push(0ll); break;
		case iPush_1: stack.push(1ll); break;
		case iPush_2: stack.push(2ll); break;
		case iPush_3: stack.push(3ll); break;
		case iPush_4: stack.push(4ll); break;
		case iPush_5: stack.push(5ll); break;
		case iPush_6: stack.push(6ll); break;
		case iPush_7: stack.push(7ll); break;

		case iPushByte1:
			stack.push((int64_t)code_ptr[1]);
			break;

		case iPushShort:
			stack.push((int64_t)(*(uint16_t*)(code_ptr + 1)));
			break;

		case iPushI24:
			stack.push((int64_t)U24_ARG(1));
			break;

		case iPushInt:
			stack.push((int64_t)(*(int32_t*)(code_ptr + 1)));
			break;

		case pGlobal3:
			stack.push(GlobalVar(U24_ARG(1)));
			break;

		case pStruct1:
			if (!stack.empty())
			{
				stack.top().val_global.offset = code_ptr[1];
			}
			break;

		case pStruct2:
			if (!stack.empty())
			{
				stack.top().val_global.offset = (*(uint16_t*)(code_ptr + 1));
			}
			break;

		case pStructStack:
		{
			auto offset = (uint32_t)stack.top().val_int;
			stack.pop();
			if (!stack.empty())
			{
				stack.top().val_global.offset = offset;
			}
		}
		break;

		case PushString:
			if (!stack.empty())
			{
				if (auto val = scr->getString((uint32_t)stack.top().val_int))
				{
					stack.pop();
					stack.push(val);
				}
			}
			break;
		}
		return true;
	}

	void ParseState::advance() noexcept
	{
		code_i += (1 + getOpcodeArgLength(getCodePtr()));
	}
}
