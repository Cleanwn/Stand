#pragma once

#include <string>

namespace Stand
{
	struct StringCastable
	{
		std::string value;

		StringCastable()
			: value({})
		{
		}

		StringCastable(const std::string& value)
			: value(value)
		{
		}

		StringCastable(std::string&& value)
			: value(std::move(value))
		{
		}

		StringCastable(const char* value)
			: value(value)
		{
		}

		StringCastable(bool value)
			: value(value ? "true" : "false")
		{
		}

		StringCastable(char value);
		StringCastable(float value);
		StringCastable(double value);
		StringCastable(int8_t value);
		StringCastable(uint8_t value);
		StringCastable(int16_t value);
		StringCastable(uint16_t value);
		StringCastable(int32_t value);
		StringCastable(uint32_t value);
		StringCastable(int64_t value);
		StringCastable(uint64_t value);
		StringCastable(void* value);

		template <typename T>
		StringCastable(const T& value)
			: value(value.operator std::string())
		{
		}

		template <typename T>
		StringCastable(T* ptr)
			: StringCastable((void*)ptr)
		{
		}

		operator std::string() const
		{
			return value;
		}

		operator std::string&()
		{
			return value;
		}

		operator const std::string& () const
		{
			return value;
		}

		operator const char* () const
		{
			return value.c_str();
		}
	};
}
