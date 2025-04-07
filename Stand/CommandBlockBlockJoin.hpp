#pragma once

#include "CommandToggleRegular.hpp"

#include <vector>

#include "gta_fwddecl.hpp"
#include "typedecl.hpp"

#include "rlGamerId.hpp"
#include "ToastConfig.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct SessionPlayer
	{
		time_t discovery;
		int64_t rid;
		std::string name;
		rage::netPeerId peerId;

		explicit SessionPlayer(const rage::rlGamerInfo& info);

		[[nodiscard]] bool operator==(const SessionPlayer& b) const noexcept;
	};

	class CommandBlockBlockJoin : public CommandToggleRegular
	{
	public:
		inline static ToastConfig toast_config{ true };
		inline static std::vector<SessionPlayer> session_players{};
		inline static std::vector<SessionPlayer> pending_object_ids{};
		inline static std::vector<SessionPlayer> pending_script_join_acks{};
		inline static bool resend_script_join = false;

		explicit CommandBlockBlockJoin(CommandList* const parent);

		void onChangeInner(Click& click) final;
	};
#pragma pack(pop)
}
