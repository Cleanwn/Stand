#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandListSessionMemberAction : public CommandAction
	{
	public:
		using CommandAction::CommandAction;

		[[nodiscard]] const CommandListSessionMember* getMember() const
		{
			return parent->as<CommandListSessionMember>();
		}

		[[nodiscard]] const CommandListSession* getSession() const
		{
			return getMember()->getSession();
		}

		[[nodiscard]] const rage::rlSessionInfo* getSessionInfoIfAvailable() const
		{
			return getMember()->getSessionInfoIfAvailable();
		}
	};
}
