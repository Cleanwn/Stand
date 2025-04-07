#pragma once

#include <string>

#include <Windows.h>

namespace Stand
{
	enum ePackedStat
	{
		PACKED_MP_BOOL_AWARD_XMAS2017_CANNON_GIFT = 18114,
		PACKED_MP_BOOL_PURCHASE_XMAS2017_CANNON_GIFT = 18115,
	};

	class StatUtil
	{
	public:
		static void setPackedBool(int index, BOOL toggle, int character);
		static void setPackedInt(int index, int value, int character);

	private:
		[[nodiscard]] static std::string getKey(const char* name);

	public:
		[[nodiscard]] static std::string getDualString(const char* name);
		static void setDualString(const char* name, const std::string& value);
	};
}
