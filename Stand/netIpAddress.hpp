#pragma once

#include "gta_fwddecl.hpp"

#include <soup/Endian.hpp>

#include "BiString.hpp"
#include "GeoIpField.hpp"

#pragma pack(push, 1)
namespace rage
{
	class netIpAddress
	{
	public:
		soup::native_u32_t value;

		constexpr netIpAddress(uint32_t&& value = -1) noexcept
			: value(std::move(value))
		{
		}

		constexpr netIpAddress(const uint8_t field1, const uint8_t field2, const uint8_t field3, const uint8_t field4) noexcept
			: netIpAddress(
				(field1 << 24) |
				(field2 << 16) |
				(field3 << 8) |
				(field4)
			)
		{
		}

		constexpr operator uint32_t() const noexcept
		{
			return value.data;
		}

		void operator =(uint32_t val) noexcept
		{
			value = val;
		}

	private:
		struct netAddress_fields
		{
			uint8_t field4;
			uint8_t field3;
			uint8_t field2;
			uint8_t field1;
		};
		static_assert(sizeof(netAddress_fields) == 4);

	public:
		template <typename T>
		[[nodiscard]] T toString() const
		{
			auto* fields = (const netAddress_fields*)this;
			auto str = ::Stand::BiString<T>::fromValue(fields->field1);
			BI_PUSH_BACK(str, '.');
			str.append(::Stand::BiString<T>::fromValue(fields->field2));
			BI_PUSH_BACK(str, '.');
			str.append(::Stand::BiString<T>::fromValue(fields->field3));
			BI_PUSH_BACK(str, '.');
			str.append(::Stand::BiString<T>::fromValue(fields->field4));
			return std::move(str);
		}

		operator std::string() const;
		operator std::wstring() const;

		[[nodiscard]] static netIpAddress fromString(const std::string& str);
		[[nodiscard]] static netIpAddress fromString(const std::wstring& str);

		[[nodiscard]] static netIpAddress random();

		[[nodiscard]] const char* getGeoIpField(Stand::GeoIpField field);
	};
	static_assert(sizeof(netIpAddress) == 4);
}
#pragma pack(pop)
