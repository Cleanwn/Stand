#pragma once

#include "struct_base.hpp"

namespace rage
{
	struct rlGamerHandle
	{
		enum Platform : uint8_t
		{
			XBOX_ONE = 1,
			PS4 = 2,
			PC_ROS = 3,
		};

		/* 0 */ int64_t rockstar_id{};
		/* 8 */ Platform platform{};
		/* 9 */ uint8_t bot_index{};
		PAD(10, 16);

		rlGamerHandle() = default;

		constexpr rlGamerHandle(int64_t rockstar_id) noexcept
			: rockstar_id(rockstar_id), platform(PC_ROS)
		{
		}

		[[nodiscard]] static rlGamerHandle fromString(const char* c) noexcept;

		[[nodiscard]] constexpr bool isValid() const noexcept
		{
			return platform == PC_ROS;
		}

		void operator= (int64_t rockstar_id) noexcept
		{
			this->rockstar_id = rockstar_id;
			this->platform = PC_ROS;
		}

		[[nodiscard]] bool operator==(const rlGamerHandle& b) const noexcept
		{
			// The game would also check the unk and if platform == PC_ROS
			return rockstar_id == b.rockstar_id
				&& platform == b.platform;
		}

		void Import(void* data) noexcept;
#ifdef STAND_DEBUG
		bool write(datBitBuffer& bb) const;
#endif

		template <typename T>
		bool ser(T& bb)
		{
			return bb.serU8(*reinterpret_cast<uint8_t*>(&platform))
				&& bb.serI64(rockstar_id)
				&& bb.serU8(bot_index)
				;
		}

		[[nodiscard]] static rlGamerHandle fromScript(void* data) noexcept;
	};
	static_assert(sizeof(rlGamerHandle) == 16);
}
