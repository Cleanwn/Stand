#include "StatUtil.hpp"

#include <fmt/core.h>

#include "atStringHash.hpp"
#include "natives.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	void StatUtil::setPackedBool(int index, BOOL toggle, int character)
	{
		STATS::SET_PACKED_STAT_BOOL_CODE(index, toggle, character);
	}

	void StatUtil::setPackedInt(int index, int value, int character)
	{
		if (index < 0)
		{
			index = 255;
		}
		STATS::SET_PACKED_STAT_INT_CODE(index, value, character);
	}

	std::string StatUtil::getKey(const char* name)
	{
		return fmt::format("mp{}_{}", ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>(), name);
	}

	std::string StatUtil::getDualString(const char* name)
	{
		std::string key = getKey(name);
		std::string val{};
		val.append(STATS::STAT_GET_STRING(rage::atStringHash(key), -1));
		key.push_back('2');
		val.append(STATS::STAT_GET_STRING(rage::atStringHash(key), -1));
		return val;
	}

	void StatUtil::setDualString(const char* name, const std::string& value)
	{
		std::string key = getKey(name);
		auto cstr = value.c_str();
		STATS::STAT_SET_STRING(rage::atStringHash(key), cstr, true);
		key.push_back('2');
		if (value.length() > 31)
		{
			STATS::STAT_SET_STRING(rage::atStringHash(key), cstr + 31, true);
		}
		else
		{
			STATS::STAT_SET_STRING(rage::atStringHash(key), "", true);
		}
	}
}
