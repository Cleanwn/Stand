#pragma once

#include "CommandListSession.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandHistoricPlayerSession : public CommandListSession
	{
	public:
		explicit CommandHistoricPlayerSession(CommandList* const parent, const CommandName& simple_name);

		bool checkCreateJoinAndSpecateCommands() final;
		[[nodiscard]] const rage::rlSessionInfo* getSessionInfoIfAvailable() const final;
		[[nodiscard]] int64_t getSessionType() const final;
	private:
		void join(long long method) const;
		void spectate(long long method) const;
	};
#pragma pack(pop)
}
