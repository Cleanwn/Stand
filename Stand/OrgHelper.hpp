#pragma once

#include "gta_player.hpp"

namespace Stand
{
	struct OrgHelper
	{
		enum JoinState : uint8_t
		{
			IDLE,
			PENDING,
			ACCEPTED,
			REJECTED,
		};

		inline static JoinState join_state = IDLE;

		static void join(player_t boss);
		[[nodiscard]] static bool hasSentJoinRequest() noexcept;
		[[nodiscard]] static player_t getBoss() noexcept;
		[[nodiscard]] static bool hasBossRespondedToJoinRequest() noexcept;
		[[nodiscard]] static bool hasBossAcceptedJoinRequest() noexcept;

		static void processJoinResponse(bool accepted) noexcept;

		static void leave() noexcept;
	};
}
