#include "script_program.hpp"

#include <fmt/core.h>

#include <soup/Range.hpp>

#include "FileLogger.hpp"
#include "pointers.hpp"
#include "script_vm_common.hpp"
#include "ScriptVmOpCode.hpp"

namespace rage
{
	using namespace Stand;

    scrProgram* scrProgram::fromHash(const int32_t hash)
    {
		for (uint64_t i = 0; i < pointers::script_programs_size; i++)
		{
			if (pointers::script_programs[i] && pointers::script_programs[i]->m_name_hash == hash)
			{
				return pointers::script_programs[i];
			}
		}
		return nullptr;
    }

	bool scrProgram::is_valid() const
	{
		return m_code_size != 0;
	}

	uint32_t scrProgram::getNumCodePages() const
	{
		return (m_code_size + 0x3FFF) >> 14;
	}

	uint32_t scrProgram::getCodePageSize(uint32_t page) const
	{
		auto num = getNumCodePages();
		if (page < num)
		{
			if (page == num - 1)
			{
				return m_code_size & 0x3FFF;
			}
			return 0x4000;
		}
		return 0;
	}

	uint32_t scrProgram::getFullCodeSize() const
	{
		auto numPages = getNumCodePages();
		if (numPages == 0)
		{
			return 0;
		}
		if (numPages == 1)
		{
			--numPages;
		}
		return (numPages * 0x4000) + (m_code_size & 0x3FFF);
	}

	uint8_t* scrProgram::getCodePage(uint32_t page) const
	{
		return m_code_blocks[page];
	}

	uint8_t* scrProgram::getCodePtr(uint32_t index) const
	{
		if (index < m_code_size)
		{
			return &getCodePage(index >> 14)[index & 0x3FFF];
		}
		return nullptr;
	}

	uint32_t scrProgram::scan(const soup::Pattern& p) const
	{
		for (uint32_t i = 0; i != getNumCodePages(); ++i)
		{
			soup::Range code_page(getCodePage(i), getCodePageSize(i));
			if (auto res = code_page.scan(p))
			{
				return (i << 14) | static_cast<uint32_t>(res.as<uint8_t*>() - getCodePage(i));
			}
		}
		return -1;
	}

	const char* scrProgram::getString(uint32_t index) const
	{
		if (index < m_strings_count)
		{
			return &m_strings_data[index >> 14][index & 0x3FFF];
		}
		return nullptr;
	}

	uint32_t scrProgram::getNativeTableIndex(rage::scrNativeHandler handler) const
	{
		for (uint32_t i = 0; i != m_native_count; ++i)
		{
			if (m_native_entrypoints[i] == handler)
			{
				return i;
			}
		}
		return -1;
	}

	std::vector<uint32_t> scrProgram::getNativeTableIndecies(rage::scrNativeHandler handler) const
	{
		std::vector<uint32_t> res;
		for (uint32_t i = 0; i != m_native_count; ++i)
		{
			if (m_native_entrypoints[i] == handler)
			{
				res.emplace_back(i);
			}
		}
		return res;
	}

	::Stand::scrFunc scrProgram::getFunc(uint32_t func_i_target)
	{
		uint32_t func_i = 0;
		uint32_t code_i = 0;
		uint8_t* op_ptr = getCodePtr(code_i);

		uint32_t func_start = 0;
		uint32_t last_ret = 0;

		while (code_i < m_code_size)
		{
			switch (*op_ptr)
			{
			case Enter:
				if (func_i == func_i_target + 1)
				{
					auto enter_len = getOpcodeArgLength(getCodePtr(func_start));
					return ::Stand::scrFunc{ func_start, func_start + enter_len, last_ret + 2 };
				}
				++func_i;
				func_start = code_i;
				break;

			case Return:
				last_ret = code_i;
				break;
			}
			code_i += (1 + getOpcodeArgLength(op_ptr));
			op_ptr = getCodePtr(code_i);
		}
		return ::Stand::scrFunc{};
	}

	uint32_t scrProgram::getFuncIndexByCodeIndex(uint32_t code_i_target)
	{
		uint32_t func_i = 0;
		uint32_t code_i = 0;
		uint8_t* op_ptr = getCodePtr(code_i);
		uint8_t* func_start = op_ptr;
		uint8_t* last_ret = nullptr;
		while (code_i < m_code_size)
		{
			switch (*op_ptr)
			{
			case Enter:
				++func_i;
				func_start = op_ptr;
				break;

			case Return:
				last_ret = op_ptr;
				break;
			}
			code_i += (1 + getOpcodeArgLength(op_ptr));
			if (code_i >= code_i_target)
			{
				return func_i;
			}
			op_ptr = getCodePtr(code_i);
		}
		return -1;
	}
}
