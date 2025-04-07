#pragma once

#include "CommandList.hpp"

#include "rlGamerInfo.hpp"
#include "ScAccount.hpp"

namespace Stand
{
	class CommandListSessionMember : public CommandList
	{
	public:
		rage::rlGamerInfo gamerInfo;
		ScAccount* account;
		time_t last_accounted_for;
		bool did_scaccount_lookup = false;

		explicit CommandListSessionMember(CommandList* const _parent, rage::rlGamerInfo&& _gamerInfo);

		[[nodiscard]] const CommandListSession* getSession() const;
		[[nodiscard]] const rage::rlSessionInfo* getSessionInfoIfAvailable() const;

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;
	};
}
