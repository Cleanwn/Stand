#include "CommandPlayerBounty.hpp"

#include <fmt/xchar.h>

#include "AbstractPlayer.hpp"
#include "CommandToggle.hpp"
#include "script_events.hpp"
#include "ScriptGlobal.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandPlayerBounty::CommandPlayerBounty(CommandList* const parent, CommandToggleNoCorrelation* bounty_self)
		: CommandPlayerActionValue(parent, 0, 10000, LOC("BNTY"), { CMDNAME("bounty") }, NOLABEL, COMMANDPERM_RUDE), bounty_self(bounty_self)
	{
	}

	void CommandPlayerBounty::onCommandWithValue(Click& click, long long value)
	{
		click.ensureScriptThread([this, value{ (int)value }]
		{
			for (const AbstractPlayer p : PP_PTR->getPlayers(bounty_self == nullptr || bounty_self->m_on))
			{
				p.giveBounty(value);
			}
		});
	}
}
