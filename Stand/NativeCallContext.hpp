#pragma once

#include "scrNativeCallContext.hpp"

namespace Stand
{
	class NativeCallContext : public rage::scrNativeCallContext
	{
	private:
		uint64_t m_return_stack[10] = { 0 };
		uint64_t m_arg_stack[100] = { 0 };

	public:
		NativeCallContext()
			: rage::scrNativeCallContext()
		{
			m_return_value = &m_return_stack[0];
			m_args = &m_arg_stack[0];
		}
	};
}
