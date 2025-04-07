#include "ScriptGlobal.hpp"

#if REPORT_OFFTHREAD_SG_WRITE
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#endif
#include "pointers.hpp"
#include "script_program.hpp"
#include "scrValue.hpp"

namespace Stand
{
	bool ScriptGlobal::isAvailable() const noexcept
	{
		auto block = m_offset >> rage::scrProgram::MAX_GLOBAL_BLOCKS_SHIFT;
		return pointers::rage_scrProgram_sm_Globals[block] != nullptr;
	}

	void* ScriptGlobal::addr() const noexcept
	{
#if REPORT_OFFTHREAD_SG_WRITE
		if (!ExecCtx::get().isScript()
			&& !g_sgtamper_readonly
			)
		{
			Exceptional::report("Possible write access to script global outside of script thread");

			// What to do if this is triggered:
			// - In case of read access: Use ScriptGlobal::get to explicitly note that you are not writing to the script global
			// - In case of write access: Ensure a script thread context
		}
#endif
		auto block = m_offset >> rage::scrProgram::MAX_GLOBAL_BLOCKS_SHIFT;
		auto ofs = m_offset & rage::scrProgram::GLOBAL_SIZE_MASK;
		return &pointers::rage_scrProgram_sm_Globals[block][ofs];
	}

	void ScriptGlobal::iterate(uint32_t elm_size, const std::function<void(uint32_t, ScriptGlobal&&)>& consumer) const
	{
		const auto arr_size = get<uint32_t>();
		for (uint32_t i = 0; i != arr_size; ++i)
		{
			consumer(i, at(i, elm_size));
		}
	}

	uint32_t ScriptGlobal::addrToOffset(void* _addr)
	{
		uintptr_t addr = reinterpret_cast<uintptr_t>(_addr);
		for (uint32_t block = 0; block != rage::scrProgram::MAX_GLOBAL_BLOCKS; ++block)
		{
			if (auto block_start = reinterpret_cast<uintptr_t>(pointers::rage_scrProgram_sm_Globals[block]))
			{
				auto block_end = block_start + rage::scrProgram::GLOBAL_SIZE_MASK;
				if (addr >= block_start && addr < block_end)
				{
					return (block << rage::scrProgram::MAX_GLOBAL_BLOCKS_SHIFT) | static_cast<uint32_t>((addr - block_start) / sizeof(rage::scrValue));
				}
			}
		}
		return -1;
	}
}
