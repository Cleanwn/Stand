#pragma once

#include <cstdint>
#include <string>

#include "rgsc_common.hpp"

namespace rgsc
{
	struct IPresenceManager : public IRgscUnknown
	{
	};

	struct IPresenceManagerV1 : public IPresenceManager
	{
		virtual bool RGSC_CALL SetIntAttribute(unsigned int localGamerIndex, const char* name, int64_t value) = 0;
		virtual bool RGSC_CALL SetDoubleAttribute(unsigned int localGamerIndex, const char* name, double value) = 0;
		virtual bool RGSC_CALL SetStringAttribute(unsigned int localGamerIndex, const char* name, const char* value) = 0;
	};
}

namespace rage
{
	enum rlScPresenceAttributeType
	{
		RLSC_PRESTYPE_INVALID,
		RLSC_PRESTYPE_S64,
		RLSC_PRESTYPE_DOUBLE,
		RLSC_PRESTYPE_STRING,
	};

	struct rlScPresenceAttribute
	{
		char name[64];
		union
		{
			int64_t int_value;
			double double_value;
			char string_value[256];
		};
		rlScPresenceAttributeType type;
		bool is_set;

		[[nodiscard]] static rlScPresenceAttribute* byIndex(uint32_t i);
		[[nodiscard]] static rlScPresenceAttribute* findByName(const char* name);

#ifdef STAND_DEBUG
		[[nodiscard]] std::string toString() const;
#endif

		void setInt(int64_t value) noexcept;
		void setString(const char* value) noexcept;
		void setString(const std::string& value) noexcept;
	};
	static_assert(sizeof(rlScPresenceAttribute) == 328);
}

using enum rage::rlScPresenceAttributeType;
