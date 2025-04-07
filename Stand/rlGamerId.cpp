#include "rlGamerId.hpp"

#if true

#include <soup/joaat.hpp>

#include "pointers.hpp"
#include "rlGamerInfo.hpp"

using namespace Stand;

namespace rage
{
	bool rlGamerId::isValid() const noexcept
	{
		return (*this >> 16) != 0;
	}

	rlGamerId rlGamerId::Generate(const std::string& name, uint64_t mac_address) noexcept
	{
		uint32_t hi = soup::joaat::hash(name, (uint32_t)(mac_address >> 32));
		uint32_t lo = soup::joaat::hash(name, (uint32_t)mac_address);
		return (((uint64_t)hi << 32) | lo);
	}

	uint32_t rlGamerId::deriveMacAddressHi(const std::string& name) const noexcept
	{
		return soup::joaat::deriveInitialNoFinalise((*this >> 32), name);
	}

	bool rlGamerId::isCongruentWithName(const std::string& name) const noexcept
	{
		return deriveMacAddressHi(name) < 0x10000;
	}

	bool rlGamerId::isCongruentWithNameAndMacId(const std::string& name, uint32_t mac_id) const noexcept
	{
		auto mac = deriveMacAddress(name);
		return isMacAddressValid(mac)
			&& hashMacAddress(mac) == mac_id
			;
	}

	uint64_t rlGamerId::deriveMacAddress(const std::string& name) const noexcept
	{
		auto mac = ((uint64_t)deriveMacAddressHi(name) << 32);
		mac |= soup::joaat::deriveInitialNoFinalise((uint32_t)*this, name);
		return mac;
	}

	bool rlGamerId::isMacAddressValid(uint64_t mac_address) noexcept
	{
		auto arr = (const uint8_t*)&mac_address;
		return arr[6] == 0
			&& arr[7] == 0
			;
	}

	uint32_t rlGamerId::hashMacAddress(uint64_t mac_address) noexcept
	{
		auto hash = soup::joaat::partial((const char*)&mac_address, 6, 0);
		soup::joaat::finalise(hash);
		return hash;
	}

	std::string rlGamerId::formatMacAddress(uint64_t mac_address) noexcept
	{
		if (!isMacAddressValid(mac_address))
		{
			return "[Garbage]";
		}
		auto arr = (const uint8_t*)&mac_address;
		return fmt::format("{:0>2X}-{:0>2X}-{:0>2X}-{:0>2X}-{:0>2X}-{:0>2X}", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
	}

	std::string rlGamerId::formatMacAddress(uint64_t mac_address, uint32_t mac_hash) noexcept
	{
		if (!isMacAddressValid(mac_address))
		{
			return "[Garbage]";
		}
		auto str = formatMacAddress(mac_address);
		if (hashMacAddress(mac_address) != mac_hash)
		{
			str.append(" [Mismatch]");
		}
		return str;
	}
}

#endif
