#include "CommandSessionBrowserSession.hpp"

#include "CommandActionSliderJoinLambda.hpp"
#include "JoinUtil.hpp"

namespace Stand
{
	const rage::rlSessionInfo* CommandSessionBrowserSession::getSessionInfoIfAvailable() const
	{
		return &sess.info;
	}

	bool CommandSessionBrowserSession::checkCreateJoinAndSpecateCommands()
	{
		if (children.empty())
		{
			this->createChild<CommandActionSliderJoinLambda>(LOC("JOIN2"), [this](long long method)
			{
				JoinUtil::join(sess.info, method);
			}, CMDNAMES(), CMDFLAG_TEMPORARY);
#if CAN_SPECTATE
			this->createChild<CommandActionSliderJoinLambda>(LOC("PLYSPEC"), [this](long long method)
			{
				JoinUtil::join(sess.info, method);
			}, CMDNAMES(), CMDFLAG_TEMPORARY);
#endif
			return true;
		}
		return false;
	}
}
