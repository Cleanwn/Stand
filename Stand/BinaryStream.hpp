#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace Stand
{
	class BinaryOutStream
	{
	private:
		std::ofstream stream;

	public:
		explicit BinaryOutStream(const std::wstring& path);

		void write(const char* data, size_t size);
		void writeVarLong(uint64_t in);
		void writeNullTerminatedString(const char* str);
		void writeNullTerminatedString(const std::string& str);
		void writeLengthPrefixedString(const std::string& str);

		template <typename T>
		void write(const T& item)
		{
			write((const char*)&item, sizeof(T));
		}

		template <typename T>
		void writeArray(const std::vector<T>& arr)
		{
			writeVarLong(arr.size());
			write((const char*)arr.data(), arr.size() * sizeof(T));
		}

		void writeArray(const std::vector<std::string>& arr) = delete;

		void writeNullTerminatedStringArray(const std::vector<std::string>& arr)
		{
			writeVarLong(arr.size());
			for (const auto& str : arr)
			{
				writeNullTerminatedString(str);
			}
		}
	};

	struct BinaryInStream
	{
		std::ifstream stream;

		explicit BinaryInStream(const std::wstring& path);

		template <typename T>
		[[nodiscard]] T read()
		{
			T item{};
			stream.read((char*)&item, sizeof(T));
			return item;
		}

		[[nodiscard]] uint64_t readVarLong();
		[[nodiscard]] std::string readNullTerminatedString();
		[[nodiscard]] std::string readLengthPrefixedString();

		[[nodiscard]] bool hasMore();
	};
}
