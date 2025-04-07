#pragma once

#include <cstdint>

#include "gta_fwddecl.hpp"
#include "natives_decl.hpp"

#include "pgBase.hpp"

#include "script_func.hpp"
#include "script_id.hpp"

#pragma pack(push, 1)
namespace rage
{
	class scrProgram : public pgBase
	{
	public:
		uint8_t** m_code_blocks;                      // 0x10
		uint32_t m_hash;                              // 0x18
		uint32_t m_code_size;                         // 0x1C
		uint32_t m_arg_count;                         // 0x20
		uint32_t m_local_count;                       // 0x24
		uint32_t m_global_count;                      // 0x28
		uint32_t m_native_count;                      // 0x2C
		void* m_local_data;                           // 0x30
		int64_t** m_global_data;                      // 0x38
		rage::scrNativeHandler* m_native_entrypoints; // 0x40
		char m_padding6[0x10];                        // 0x48
		hash_t m_name_hash;                           // 0x58
		char m_padding7[0x04];                        // 0x5C
		const char* m_name;                           // 0x60
		const char** m_strings_data;                  // 0x68
		uint32_t m_strings_count;                     // 0x70
		char m_padding8[0x0C];                        // 0x74

		[[nodiscard]] static scrProgram* fromHash(const int32_t hash);

		[[nodiscard]] bool is_valid() const;

		[[nodiscard]] uint32_t getNumCodePages() const;
		[[nodiscard]] uint32_t getCodePageSize(uint32_t page) const;
		[[nodiscard]] uint32_t getFullCodeSize() const;
		[[nodiscard]] uint8_t* getCodePage(uint32_t page) const;
		[[nodiscard]] uint8_t* getCodePtr(uint32_t index) const;
		[[nodiscard]] uint32_t scan(const soup::Pattern& p) const;

		[[nodiscard]] const char* getString(uint32_t index) const;

		[[nodiscard]] uint32_t getNativeTableIndex(rage::scrNativeHandler handler) const;
		[[nodiscard]] std::vector<uint32_t> getNativeTableIndecies(rage::scrNativeHandler handler) const;

		[[nodiscard]] ::Stand::scrFunc getFunc(uint32_t func_i_target);
		[[nodiscard]] uint32_t getFuncIndexByCodeIndex(uint32_t code_i_target);

		enum {
			MAX_GLOBAL_BLOCKS_SHIFT = (24 - 6),
			MAX_GLOBAL_BLOCKS = (64),
			GLOBAL_SIZE_MASK = ((1 << (MAX_GLOBAL_BLOCKS_SHIFT)) - 1)
		};
	};
	static_assert(sizeof(scrProgram) == 0x80);

	class scrProgramTableEntry
	{
	public:
		scrProgram* m_program;     // 0x00
		char m_Pad1[0x04];         // 0x08
		int32_t m_hash;            // 0x0C
	};

	class scrProgramTable
	{
	public:
		scrProgramTableEntry* m_data;    // 0x00
		char m_padding[0x10];            // 0x08
		uint32_t m_size;                 // 0x18

		scrProgram* find_script(int32_t hash)
		{
			for (uint32_t i = 0; i < m_size; ++i)
			{
				if (m_data[i].m_hash == hash)
				{
					return m_data[i].m_program;
				}
			}

			return nullptr;
		}

		scrProgramTableEntry* begin()
		{
			return m_data;
		}

		scrProgramTableEntry* end()
		{
			return m_data + m_size;
		}
	};

	static_assert(sizeof(scrProgram) == 0x80);
	static_assert(sizeof(scrProgramTableEntry) == 0x10);
	static_assert(sizeof(scrProgramTable) == 0x1C);
}
#pragma pack(pop)

