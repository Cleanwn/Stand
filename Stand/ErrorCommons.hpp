#pragma once

#include <windows.h>

namespace Stand
{
	extern "C"
	{
		// get_stack_pointer.asm
		void** get_stack_pointer();
	}

	struct ErrorInfo
	{
		bool is_exp;
		union
		{
			_EXCEPTION_POINTERS* exp;
			void** sp;
		};

		ErrorInfo(_EXCEPTION_POINTERS* exp)
			: is_exp(true), exp(exp)
		{
		}

		__forceinline explicit ErrorInfo(void** sp)
			: is_exp(false), sp(sp)
		{
		}

		__forceinline static ErrorInfo fromStack()
		{
			return ErrorInfo(get_stack_pointer());
		}
	};
}
