#pragma once

#include <cstdint>
#include <string>

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"

#include "conf_netcode.hpp"

#include "JoinMethod.hpp"
#include "rlSessionInfo.hpp"

namespace Stand
{
	struct JoinHint
	{
		int64_t rid = 0;
		rage::rlSessionInfo session_info{};

		JoinHint(int64_t rid)
			: rid(rid)
		{
		}

		JoinHint(rage::rlSessionInfo session_info)
			: session_info(std::move(session_info))
		{
		}

		[[nodiscard]] bool hasSessionInfo() const noexcept
		{
			return session_info.id != 0
				&& session_info.token != 0
				;
		}

		[[nodiscard]] int64_t getRid() const noexcept
		{
			if (hasSessionInfo())
			{
				return session_info.host.gamer_handle.rockstar_id;
			}
			return rid;
		}
		
		[[nodiscard]] bool ensureSessionInfo(bool silent = false);
		[[nodiscard]] bool onPreJoin(bool method_has_no_dialogs = false, bool rejoin = false);
	};

	class JoinUtil
	{
	public:
		// Join/Spectate - Config

		inline static bool auto_switch_targeting = false;
#if I_CAN_SPY
		inline static bool rejectblock = false;
#endif

		// Join/Spectate - Flow initiators

		static void join(int64_t rid, long long method = JM_DEFAULT)
		{
			connect(method, rid, false);
		}

		static void join(rage::rlSessionInfo session_info, long long method = JM_DEFAULT)
		{
			connect(method, std::move(session_info), false);
		}

		static void spectate(int64_t rid, long long method = JM_DEFAULT)
		{
			connect(method, rid, true);
		}

		static void spectate(rage::rlSessionInfo session_info, long long method = JM_DEFAULT)
		{
			connect(method, std::move(session_info), true);
		}

		static void connect(rage::rlSessionInfo session_info, bool as_spectator)
		{
			connect(JM_DEFAULT, std::move(session_info), as_spectator);
		}

		static void connectViaCode(const std::string& code, bool as_spectator);

		static void connect(long long method, JoinHint&& hint, bool as_spectator);

		// Join/Spectate - Private functions, to be called inside a script thread fiber

	private:
		static void dispatch(long long method, JoinHint& hint, bool as_spectator);

		static void dispatchNinja(JoinHint& hint, bool as_spectator);
		static void dispatchFriend(JoinHint& hint, bool as_spectator);
		static void dispatchMatchmaking(JoinHint& hint, bool as_spectator);
		static void dispatchNuts(JoinHint& hint, bool as_spectator, bool rejoin);

		// Invite

	public:
		static bool inviteViaRidPreflightCheck(Click& click);
		static void inviteViaRid(Click& click, int64_t rid);
		static void inviteViaRid(int64_t rid);

		// Utilities

		[[nodiscard]] static std::string parseCode(std::wstring& args);

	private:
		static CPlayerListMenu* fiberOpenFriendsList();
	};
}
