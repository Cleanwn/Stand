#pragma once

#include <cstdint>

#include <Windows.h>

#include "gta_fwddecl.hpp"

#define BIT_BUFFER_SIZE(bits) (((bits) + 7) >> 3)

static_assert(BIT_BUFFER_SIZE(18) == 3);

#pragma pack(push, 1)
namespace rage
{
	class datBitBuffer
	{
	public:
		/* 0x00 */ void* data = nullptr;
		/* 0x08 */ uint32_t unk_08 = 0;
		/* 0x0C */ uint32_t size; // size of data in bits
		/* 0x10 */ int32_t cursor_pos = 0;
		/* 0x14 */ int32_t bit_length = 0;
		/* 0x18 */ uint32_t unk_18 = 0;
		/* 0x1C */ uint8_t flags = 0; // 1 = read only, 2 = measure size only

		void SetReadOnlyBytes(const void* data, uint32_t size_bytes) noexcept;
		void SetReadWriteBytes(void* data, uint32_t size_bytes) noexcept;

		[[nodiscard]] bool isWrite() const noexcept;
		[[nodiscard]] bool isReadonly() const noexcept;
		[[nodiscard]] bool isMeasureSize() const noexcept;

		[[nodiscard]] int32_t GetCursorPos() noexcept;

		[[nodiscard]] int32_t getBitLength() noexcept;
		[[nodiscard]] unsigned int GetByteLength() noexcept;
		[[nodiscard]] unsigned int GetNumBytesWritten() noexcept;

		void SetCursorPos(int32_t bits) noexcept;
		void seekStart() noexcept;
		void seekEnd() noexcept;

		bool deserialiseBool(bool& val) noexcept;
		bool deserialiseU8(uint8_t& val, unsigned int bits = 8) noexcept;
		bool deserialiseU16(uint16_t& val) noexcept;
		bool deserialiseU32(uint32_t& val, unsigned int bits = 32) noexcept;
		bool deserialiseI32(int32_t& val, unsigned int bits = 32) noexcept;
		bool deserialiseU64(uint64_t& val, unsigned int bits = 64) noexcept;
		bool deserialiseI64(int64_t& val, unsigned int bits = 64) noexcept;

		[[nodiscard]] bool readBool() noexcept;
		[[nodiscard]] uint8_t readU8(unsigned int bits = 8) noexcept;
		[[nodiscard]] uint16_t readU16(unsigned int bits = 16) noexcept;
		[[nodiscard]] uint32_t readU32(unsigned int bits = 32) noexcept;
		[[nodiscard]] int16_t readI16(unsigned int bits = 16) noexcept;
		[[nodiscard]] int32_t readI32(unsigned int bits = 32) noexcept;
		[[nodiscard]] uint64_t readU64(unsigned int bits = 64) noexcept;
		[[nodiscard]] int64_t readI64(unsigned int bits = 64) noexcept;

		[[nodiscard]] float readFloat(unsigned int bits, float divisor) noexcept;
		[[nodiscard]] float readSignedFloat(unsigned int bits, float divisor) noexcept;

		[[nodiscard]] Stand::v3 readPosition(unsigned int bits) noexcept;
		[[nodiscard]] Stand::v3 readVector(unsigned int bits, const float max_len) noexcept;

		bool ReadBits(void* arr, unsigned int bits) noexcept;
		bool ReadBytes(void* arr, unsigned int bytes) noexcept;
		bool ReadStr(char* str, unsigned int max_chars);

		void SkipBits(int bits) noexcept;

		[[nodiscard]] static uint32_t static_readU32(char*& dp) noexcept;
		[[nodiscard]] static uint64_t static_readU64(char*& dp) noexcept;

		void BumpReadWriteCursor(int bits);

		bool writeBool(bool val);
		bool writeI32(int val, int bits = 32);
		bool writeU32(unsigned int val, int bits = 32);
		bool writeI64(int64_t val, int bits = 64);
		bool writeU64(uint64_t val, int bits = 64);
		bool WriteBits(const void* arr, unsigned int bits);
		bool WriteBytes(const void* arr, unsigned int bytes);
		bool WriteStr(const char* str, unsigned int max_chars);

		[[nodiscard]] static uint32_t StrBitLen(const char* str);

		static void static_writeU32(char*& dp, uint32_t value) noexcept;
		static void static_writeU64(char*& dp, uint64_t value) noexcept;
	};
	static_assert(sizeof(datBitBuffer) == 0x1C + 1);

	class datExportBuffer : public datBitBuffer
	{
	public:
		using datBitBuffer::datBitBuffer;

		[[nodiscard]] static constexpr bool isExport() noexcept
		{
			return true;
		}

		[[nodiscard]] static constexpr bool isImport() noexcept
		{
			return false;
		}

		bool serBool(bool& val)
		{
			return writeBool(val);
		}

		bool serU8(uint8_t& val, unsigned int bits = 8)
		{
			return writeU32(val, bits);
		}
		
		bool serU16(uint16_t& val)
		{
			return writeU32(val, 16);
		}

		bool serU32(uint32_t& val, unsigned int bits = 32)
		{
			return writeU32(val, bits);
		}
		
		bool serU64(uint64_t& val)
		{
			return writeU64(val);
		}

		bool serI32(int32_t& val, unsigned int bits = 32)
		{
			return writeI32(val, bits);
		}

		bool serI64(int64_t& val)
		{
			return writeI64(val);
		}

		bool serBytes(void* arr, unsigned int bytes)
		{
			return WriteBytes(arr, bytes);
		}

		bool serStr(char* str, unsigned int max_chars)
		{
			return WriteStr(str, max_chars);
		}
	};

	class datImportBuffer : public datBitBuffer
	{
	public:
		using datBitBuffer::datBitBuffer;

		[[nodiscard]] static constexpr bool isExport() noexcept
		{
			return false;
		}

		[[nodiscard]] static constexpr bool isImport() noexcept
		{
			return true;
		}

		bool serBool(bool& val)
		{
			return deserialiseBool(val);
		}

		bool serU8(uint8_t& val, unsigned int bits = 8)
		{
			return deserialiseU8(val, bits);
		}

		bool serU16(uint16_t& val)
		{
			return deserialiseU16(val);
		}

		bool serU32(uint32_t& val, unsigned int bits = 32)
		{
			return deserialiseU32(val, bits);
		}

		bool serU64(uint64_t& val)
		{
			return deserialiseU64(val);
		}

		bool serI32(int32_t& val, unsigned int bits = 32)
		{
			return deserialiseI32(val, bits);
		}

		bool serI64(int64_t& val)
		{
			return deserialiseI64(val);
		}

		bool serBytes(void* arr, unsigned int bytes)
		{
			return ReadBytes(arr, bytes);
		}

		bool serStr(char* str, unsigned int max_chars)
		{
			return ReadStr(str, max_chars);
		}
	};
}
#pragma pack(pop)
