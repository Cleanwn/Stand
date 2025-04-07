#include "CommandRepeatTeleport.hpp"

#include "TpUtil.hpp"

namespace Stand
{
	CommandRepeatTeleport::CommandRepeatTeleport(CommandList* parent)
		: CommandAction(parent, LOC("RPLTP"), { CMDNAME("repeattp"), CMDNAME("repeatp"), CMDNAME("repeatteleport"), CMDNAME("repeateleport"), CMDNAME("redotp"), CMDNAME("redoteleport") })
	{
	}

	void CommandRepeatTeleport::onClick(Click& click)
	{
		if (!TpUtil::last_tp.has_value())
		{
			click.setResponse(LOC("RP_E"));
			return;
		}
		ensureScriptThread(click, []
		{
			TpUtil::teleport_exact(TpUtil::last_tp.value());
		});
	}
}
