#pragma once

#include "CommandListWithOnTickAsActiveList.hpp"
#include "CommandList.hpp"

#include <unordered_set>

#include "MatchmakingConfig.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandSessionBrowser : public CommandListWithOnTickAsActiveList<CommandList>
	{
	private:
		MatchmakingConfig config;
		MatchmakingConfig results_config;
		CommandDivider* results_divider;
		size_t results_offset;
	public:
		bool request_sent = false;
		bool want_refresh = true;
	private:
		std::unordered_set<hash_t> discovered_sessions{};
		size_t total_players = 0;
		bool diminishing_returns = false;

	public:
		explicit CommandSessionBrowser(CommandList* const parent);

		[[nodiscard]] size_t getNumSessions() const noexcept;

		void onClick(Click& click) final;

	protected:
		void onTickAsActiveList() final;
	};
#pragma pack(pop)
}
