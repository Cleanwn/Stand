#include "ScriptLocal.hpp"

#include "script_thread.hpp"

namespace Stand
{
	ScriptLocal::ScriptLocal(rage::scrThread* thread, std::size_t index) :
		m_index(index), m_stack(thread->m_stack)
	{
	}

	ScriptLocal::ScriptLocal(PVOID stack, std::size_t index) :
		m_index(index), m_stack(stack)
	{
	}

	ScriptLocal ScriptLocal::at(std::ptrdiff_t index)
	{
		return ScriptLocal(m_stack, m_index + index);
	}

	ScriptLocal ScriptLocal::at(std::ptrdiff_t index, std::size_t size)
	{
		return ScriptLocal(m_stack, m_index + 1 + (index * size));
	}

	void* ScriptLocal::get()
	{
		return reinterpret_cast<uintptr_t*>((uintptr_t)m_stack + (m_index * sizeof(uintptr_t)));
	}
}
