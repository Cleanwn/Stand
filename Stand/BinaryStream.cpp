#include "BinaryStream.hpp"

namespace Stand
{
	BinaryOutStream::BinaryOutStream(const std::wstring& path)
		: stream(path, std::ios::binary | std::ios::out)
	{
	}

	BinaryInStream::BinaryInStream(const std::wstring& path)
		: stream(path, std::ios::binary | std::ios::in)
	{
	}

	void BinaryOutStream::write(const char* data, size_t size)
	{
		stream.write(data, size);
	}

	void BinaryOutStream::writeVarLong(uint64_t in)
	{
		for (uint8_t i = 0; i < 8; i++)
		{
			uint8_t cur = (in & 0x7F);
			in >>= 7;
			if (in != 0)
			{
				cur |= 0x80;
			}
			write<uint8_t>(cur);
			if (in == 0)
			{
				return;
			}
		}
		if (in != 0)
		{
			write<uint8_t>((uint8_t)in);
		}
	}

	uint64_t BinaryInStream::readVarLong()
    {
		uint64_t out = 0;
		uint8_t bits = 0;
		while (true)
		{
			auto b = read<uint8_t>();
			bool has_next = false;
			if ((bits < (64 - 8)) && (b & 0x80))
			{
				has_next = true;
				b &= 0x7F;
			}
			out |= ((uint64_t)b << bits);
			if (!has_next)
			{
				break;
			}
			bits += 7;
		}
		return out;
    }

	void BinaryOutStream::writeNullTerminatedString(const char* str)
	{
		stream.write(str, strlen(str));
		write<char>(0);
	}

	void BinaryOutStream::writeNullTerminatedString(const std::string& str)
	{
		writeNullTerminatedString(str.c_str());
	}

    std::string BinaryInStream::readNullTerminatedString()
    {
		std::string str{};
		while (true)
		{
			auto c = read<char>();
			if (c == 0)
			{
				break;
			}
			str.push_back(c);
		}
        return str;
    }

	void BinaryOutStream::writeLengthPrefixedString(const std::string& str)
	{
		writeVarLong(str.size());
		stream.write(str.data(), str.size());
	}

	std::string BinaryInStream::readLengthPrefixedString()
	{
		auto len = readVarLong();
		auto data = new char[len];
		stream.read(data, len);
		std::string str(data, len);
		delete[] data;
		return str;
	}

	bool BinaryInStream::hasMore()
	{
		return stream.peek() != EOF;
	}
}
