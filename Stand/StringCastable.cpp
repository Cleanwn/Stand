#include "StringCastable.hpp"

#include <fmt/format.h>

namespace Stand
{
	StringCastable::StringCastable(char value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(float value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(double value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(int8_t value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(uint8_t value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(int16_t value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(uint16_t value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(int32_t value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(uint32_t value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(int64_t value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(uint64_t value)
		: value(fmt::to_string(value))
	{
	}

	StringCastable::StringCastable(void* value)
		: value(fmt::to_string(value))
	{
	}
}
