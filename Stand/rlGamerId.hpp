#pragma once

#if true // rlGamerId was removed in 2944

#include <string>

#include <soup/IntStruct.hpp>

namespace rage
{
	struct rlGamerId : public ::soup::IntStruct<uint64_t>
	{
		using IntStruct::IntStruct;

		[[nodiscard]] bool isValid() const noexcept;

		[[nodiscard]] static rlGamerId Generate(const std::string& name, uint64_t mac_address) noexcept;

		[[nodiscard]] uint32_t deriveMacAddressHi(const std::string& name) const noexcept;
		[[nodiscard]] bool isCongruentWithName(const std::string& name) const noexcept;
		[[nodiscard]] bool isCongruentWithNameAndMacId(const std::string& name, uint32_t mac_id) const noexcept;
		[[nodiscard]] uint64_t deriveMacAddress(const std::string& name) const noexcept;
		
		[[nodiscard]] static bool isMacAddressValid(uint64_t mac_address) noexcept;
		[[nodiscard]] static uint32_t hashMacAddress(uint64_t mac_address) noexcept;
		[[nodiscard]] static std::string formatMacAddress(uint64_t mac_address) noexcept;
		[[nodiscard]] static std::string formatMacAddress(uint64_t mac_address, uint32_t mac_hash) noexcept;
	};
}

#endif
