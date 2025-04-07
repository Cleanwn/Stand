#pragma once

#include <cstdint>
#include <utility>

#include "xormagics.hpp"

namespace Stand
{
	struct XoredNativeHash
	{
		uint64_t xored;

		constexpr XoredNativeHash() = default;

		constexpr XoredNativeHash(uint64_t hash) noexcept
			: xored(hash ^ MAGIC_SCRCMD)
		{
		}

		XoredNativeHash(const XoredNativeHash& b) noexcept
			: xored(b.xored)
		{
		}

		void operator =(uint64_t hash) = delete;

		void operator =(const XoredNativeHash& b) noexcept
		{
			xored = b.xored;
		}

		[[nodiscard]] uint64_t getHash() const noexcept
		{
			return xored ^ MAGIC_SCRCMD;
		}

		[[nodiscard]] bool operator ==(const XoredNativeHash& b) const noexcept
		{
			return xored == b.xored;
		}

		[[nodiscard]] bool operator !=(const XoredNativeHash& b) const noexcept
		{
			return !operator==(b);
		}

		void reset()
		{
			xored = 0;
		}

		uint64_t operator & (int b) const noexcept
		{
			return xored & b;
		}
	};
}

namespace std
{
	template <>
	struct hash<::Stand::XoredNativeHash>
	{
		std::size_t operator()(const ::Stand::XoredNativeHash& v) const
		{
			return v.xored;
		}
	};
}
