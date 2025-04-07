#include "CommandHistoricPlayerSession.hpp"

#include "AbstractPlayer.hpp"
#include "CommandActionSliderJoinLambda.hpp"
#include "CommandHistoricPlayer.hpp"
#include "JoinUtil.hpp"
#include "RemoteGamer.hpp"

namespace Stand
{
	CommandHistoricPlayerSession::CommandHistoricPlayerSession(CommandList* const parent, const CommandName& simple_name)
		: CommandListSession(parent, LOC("SESS"), std::vector<CommandName>(1, std::move(CommandName(CMDNAME("historysession")).append(simple_name))), NOLABEL, CMDFLAGS_LIST | CMDFLAG_TEMPORARY)
	{
		this->createChild<CommandActionSliderJoinLambda>(LOC("JOIN2"), [this](long long method)
		{
			this->join(method);
		}, std::vector<CommandName>(1, std::move(CommandName(CMDNAME("historyjoin")).append(simple_name))), CMDFLAG_TEMPORARY);
#if CAN_SPECTATE
		this->createChild<CommandActionSliderJoinLambda>(LOC("PLYSPEC"), [this](long long method)
		{
			this->spectate(method);
		}, std::vector<CommandName>(1, std::move(CommandName(CMDNAME("historyspectate")).append(simple_name))), CMDFLAG_TEMPORARY);
#endif
	}

	bool CommandHistoricPlayerSession::checkCreateJoinAndSpecateCommands()
	{
#if CAN_SPECTATE
		return children.size() == 2;
#else
		return children.size() == 1;
#endif
	}

	const rage::rlSessionInfo* CommandHistoricPlayerSession::getSessionInfoIfAvailable() const
	{
		if (auto g = parent->as<CommandHistoricPlayer>()->update())
		{
			if (g->hasSession())
			{
				return &g->session_info;
			}
		}
		return nullptr;
	}

	int64_t CommandHistoricPlayerSession::getSessionType() const
	{
		if (auto g = parent->as<CommandHistoricPlayer>()->update())
		{
			if (auto opt = g->getGsType(); opt.has_value())
			{
				return opt.value();
			}
		}
		return CommandListSession::getSessionType();
	}

	void CommandHistoricPlayerSession::join(long long method) const
	{
		Click click(CLICK_AUTO);
		if (click.isBasicEdition())
		{
			JoinUtil::join(parent->as<CommandHistoricPlayer>()->player->rid, method);
		}
	}

	void CommandHistoricPlayerSession::spectate(long long method) const
	{
		Click click(CLICK_AUTO);
		if (auto cmd = parent->as<CommandHistoricPlayer>()->player->inSession().getCommandAsList())
		{
			cmd->recursivelyResolveChildByMenuName(LOC("PLYSPEC"))->as<CommandList>()->redirectOpen(click, this);
			return;
		}
		if (click.isBasicEdition())
		{
			JoinUtil::spectate(parent->as<CommandHistoricPlayer>()->player->rid, method);
		}
	}
}
