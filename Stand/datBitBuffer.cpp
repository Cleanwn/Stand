#include "datBitBuffer.hpp"

#include "pointers.hpp"
#include "rlGamerHandle.hpp"
#include "rlGamerId.hpp"

namespace rage
{
	using namespace Stand;

	void datBitBuffer::SetReadOnlyBytes(const void* data, uint32_t size_bytes) noexcept
	{
		this->data = const_cast<void*>(data);
		this->size = size_bytes * 8;
		flags = 1;
	}

	void datBitBuffer::SetReadWriteBytes(void* data, uint32_t size_bytes) noexcept
	{
		this->data = data;
		this->size = size_bytes * 8;
	}

	bool datBitBuffer::isWrite() const noexcept
	{
		return !isReadonly();
	}

	bool datBitBuffer::isReadonly() const noexcept
	{
		return (flags & 1);
	}

	bool datBitBuffer::isMeasureSize() const noexcept
	{
		return (flags & 2);
	}

	int32_t datBitBuffer::GetCursorPos() noexcept
	{
		return cursor_pos;
	}

	int32_t datBitBuffer::getBitLength() noexcept
	{
		return isReadonly() ? size : bit_length;
	}

	unsigned int datBitBuffer::GetByteLength() noexcept
	{
		return (unsigned int)((this->bit_length + 7) >> 3);
	}

	unsigned int datBitBuffer::GetNumBytesWritten() noexcept
	{
		return (unsigned int)((this->bit_length + 7) >> 3);
	}

	void datBitBuffer::SetCursorPos(int32_t bits) noexcept
	{
		if (bits >= 0 && bits <= getBitLength())
		{
			cursor_pos = bits;
		}
	}

	void datBitBuffer::seekStart() noexcept
	{
		cursor_pos = 0;
	}

	void datBitBuffer::seekEnd() noexcept
	{
		cursor_pos = getBitLength();
	}

	bool datBitBuffer::deserialiseBool(bool& val) noexcept
	{
		uint32_t tmp;
		if (deserialiseU32(tmp, 1))
		{
			val = tmp;
			return true;
		}
		return false;
	}

	bool datBitBuffer::deserialiseU8(uint8_t& val, unsigned int bits) noexcept
	{
		uint32_t tmp;
		if (deserialiseU32(tmp, bits))
		{
			val = tmp;
			return true;
		}
		return false;
	}

	bool datBitBuffer::deserialiseU16(uint16_t& val) noexcept
	{
		uint32_t tmp;
		if (deserialiseU32(tmp, 16))
		{
			val = tmp;
			return true;
		}
		return false;
	}

	bool datBitBuffer::deserialiseU32(uint32_t& val, unsigned int bits) noexcept
	{
		return pointers::read_bitbuffer_dword(this, val, bits); // Checks state then calls ReadUnsigned and BumpReadCursor
	}

	bool datBitBuffer::deserialiseI32(int32_t& val, unsigned int bits) noexcept
	{
		uint32_t v8;
		uint32_t v9;
		if (deserialiseU32(v8, 1)
			&& deserialiseU32(v9, bits - 1u)
			)
		{
			val = (v8 + (v9 ^ -(int32_t)v8));
			return true;
		}
		return false;
	}

	bool datBitBuffer::deserialiseU64(uint64_t& val, unsigned int bits) noexcept
	{
		if (bits <= 32)
		{
			uint32_t tmp;
			if (deserialiseU32(tmp, bits))
			{
				val = (uint64_t)tmp;
				return true;
			}
		}
		else
		{
			uint32_t lo, hi;
			if (deserialiseU32(lo, 32u)
				&& deserialiseU32(hi, bits - 32u)
				)
			{
				val = ((uint64_t)hi << 32) | lo;
				return true;
			}
		}
		return false;
	}

	bool datBitBuffer::deserialiseI64(int64_t& val, unsigned int bits) noexcept
	{
		uint32_t v8;
		uint64_t v9;
		if (deserialiseU32(v8, 1)
			&& deserialiseU64(v9, bits - 1u)
			)
		{
			val = (v8 + (v9 ^ -(int64_t)v8));
			return true;
		}
		return false;
	}

	bool datBitBuffer::readBool() noexcept
	{
		return readU8(1) != 0;
	}

	uint8_t datBitBuffer::readU8(unsigned int bits) noexcept
	{
		return (uint8_t)readU32(bits);
	}

	uint16_t datBitBuffer::readU16(unsigned int bits) noexcept
	{
		return (uint16_t)readU32(bits);
	}

	uint32_t datBitBuffer::readU32(unsigned int bits) noexcept
	{
		uint32_t val;
		deserialiseU32(val, bits);
		return val;
	}

	int16_t rage::datBitBuffer::readI16(unsigned int bits) noexcept
	{
		return (int16_t)readI32(bits);
	}

	int32_t datBitBuffer::readI32(unsigned int bits) noexcept
	{
		int32_t val;
		deserialiseI32(val, bits);
		return val;
	}

	uint64_t datBitBuffer::readU64(unsigned int bits) noexcept
	{
		uint64_t val;
		deserialiseU64(val, bits);
		return val;
	}

	int64_t datBitBuffer::readI64(unsigned int bits) noexcept
	{
		int64_t val;
		deserialiseI64(val, bits);
		return val;
	}

	float datBitBuffer::readFloat(unsigned int bits, float divisor) noexcept
	{
		auto value = readU32(bits);
		const int max = (1 << bits) - 1;
		return ((float)value / (float)max) * divisor;
	}

	float datBitBuffer::readSignedFloat(unsigned int bits, float divisor) noexcept
	{
		auto value = readI32(bits);
		const int max = ((1 << (bits - 1)) - 1);
		return ((float)value / (float)max) * divisor;
	}

	v3 datBitBuffer::readPosition(unsigned int bits) noexcept
	{
		v3 pos{ 0.0f, 0.0f, 0.0f };

		pos.x = readSignedFloat(bits, 27648.0f); // a.k.a, SerialisePackedFloat
		pos.y = readSignedFloat(bits, 27648.0f);
		pos.z = readFloat(bits, 4416.0f) - 1700.0f; // a.k.a, SerialisePackedUnsignedFloat

		return pos;
	}

	v3 datBitBuffer::readVector(unsigned int bits, const float max_len) noexcept
	{
		v3 pos{ 0.0f, 0.0f, 0.0f };

		pos.x = readSignedFloat(bits, max_len);
		pos.y = readSignedFloat(bits, max_len);
		pos.z = readSignedFloat(bits, max_len);

		return pos;
	}

	bool datBitBuffer::ReadBits(void* arr, unsigned int bits) noexcept
	{
		return pointers::rage_datBitBuffer_ReadBits(this, arr, bits, 0);
	}

	bool datBitBuffer::ReadBytes(void* arr, unsigned int bytes) noexcept
	{
		return ReadBits(arr, bytes * 8);
	}

	bool datBitBuffer::ReadStr(char* str, unsigned int max_chars)
	{
		str[0] = '\0';

		uint32_t flag, num_chars;
		return deserialiseU32(flag, 1)
			&& deserialiseU32(num_chars, flag ? 15 : 7)
			&& num_chars <= max_chars
			&& ReadBytes(str, num_chars)
			&& str[num_chars - 1] == '\0'
			;
	}

	void datBitBuffer::SkipBits(int bits) noexcept
	{
		this->cursor_pos += bits;
		if (this->cursor_pos > this->bit_length)
		{
			this->bit_length = this->cursor_pos;
		}
	}

	uint32_t datBitBuffer::static_readU32(char*& dp) noexcept
	{
		// untested

		uint32_t value = (uint32_t(uint8_t(*dp++)) << 24);
		value |= (uint32_t(uint8_t(*dp++)) << 16);
		value |= (uint32_t(uint8_t(*dp++)) << 8);
		value |= (uint32_t(uint8_t(*dp++)));
		return value;
	}

	uint64_t datBitBuffer::static_readU64(char*& dp) noexcept
	{
		// should work if static_readU32 works

		uint32_t hi = static_readU32(dp);
		uint32_t lo = static_readU32(dp);
		return ((uint64_t(hi) << 32) | lo);
	}

	void datBitBuffer::BumpReadWriteCursor(int bits)
	{
		cursor_pos += bits;
		if (cursor_pos >= bit_length)
		{
			bit_length = cursor_pos;
		}
	}

	bool datBitBuffer::writeBool(bool val)
	{
		return writeU32(val, 1);
	}

	bool datBitBuffer::writeI32(int val, int bits)
	{
		int v5 = val >> 31;
		return writeU32(v5, 1)
			&& writeU32(v5 + (val ^ -v5), bits - 1)
			;
	}

	bool datBitBuffer::writeU32(unsigned int val, int bits)
	{
		if ((unsigned int)(getBitLength() + bits) > size)
		{
			return false;
		}
		if (!isMeasureSize())
		{
			// I've tried to reimplement WriteUnsigned based on the IDA disassembly, but that didn't work.
			pointers::rage_datBitBuffer_WriteUnsigned(reinterpret_cast<uint8_t*>(data), val, bits, cursor_pos + unk_08);
		}
		BumpReadWriteCursor(bits);
		return true;
	}

	bool datBitBuffer::writeI64(int64_t val, int bits)
	{
		int v5 = val >> 63;
		return writeU32(v5, 1)
			&& writeU64(v5 + (val ^ -v5), bits - 1)
			;
	}

	bool datBitBuffer::writeU64(uint64_t val, int bits)
	{
		if (!writeU32((unsigned int)val, 32))
		{
			return false;
		}
		if (bits > 32)
		{
			if (!writeU32(val >> 32, bits - 32))
			{
				return false;
			}
		}
		return true;
	}

	bool datBitBuffer::WriteBits(const void* arr, unsigned int bits)
	{
		return pointers::rage_datBitBuffer_WriteBits(this, arr, bits, 0);
	}

	bool datBitBuffer::WriteBytes(const void* arr, unsigned int bytes)
	{
		return WriteBits(arr, bytes * 8);
	}

	bool datBitBuffer::WriteStr(const char* str, unsigned int max_chars)
	{
		if (cursor_pos + StrBitLen(str) <= size)
		{
			size_t len = strlen(str);
			uint32_t size = (uint32_t)len + 1;
			if (size > max_chars)
			{
				size = max_chars;
			}
			return writeBool(size > 127)
				&& writeU32(size, size > 127 ? 15 : 7)
				&& (size == 0 || WriteBits(str, size * 8))
				;
		}
		return false;
	}

	uint32_t datBitBuffer::StrBitLen(const char* str)
	{
		size_t len = strlen(str);
		uint32_t size = (uint32_t)len + 1;
		uint32_t bits = 8 * size;
		if (size <= 127)
			return 1 + 7 + bits;
		else
			return 1 + 15 + bits;
	}

	void datBitBuffer::static_writeU32(char*& dp, uint32_t value) noexcept
	{
		*dp++ = uint8_t(value >> 24);
		*dp++ = uint8_t(value >> 16);
		*dp++ = uint8_t(value >> 8);
		*dp++ = uint8_t(value);
	}

	void datBitBuffer::static_writeU64(char*& dp, uint64_t value) noexcept
	{
		static_writeU32(dp, uint32_t(value));
		static_writeU32(dp, uint32_t(value >> 32));
	}
}
