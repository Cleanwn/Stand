#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "fwddecl.hpp"

#include "rlSessionInfo.hpp"

namespace Stand
{
	class SessionSpoofing
	{
	public:
		inline static RemoteGamer* copy_from = nullptr;
		inline static rage::rlSessionInfo random_session_info{};
		inline static std::string og_peeraddr{};

		[[nodiscard]] static rage::rlSessionInfo& getRealSessionInfo();
		[[nodiscard]] static uint64_t getRealSessionId();
		[[nodiscard]] static int64_t getRealSessionType();

		[[nodiscard]] static bool isActive() noexcept;

		static void onDisable();
	private:
		static void cleanup();
	public:
		static void onTick();
	private:
		static void onTickActive();
	};
}
