#include "CommandScriptHost.hpp"

#include "AbstractPlayer.hpp"
#include "ScriptHostUtil.hpp"

namespace Stand
{
	CommandScriptHost::CommandScriptHost(CommandList* const parent)
		: CommandAction(parent, LOC("BESCRHST"), CMDNAMES_OBF("scripthost"))
	{
	}

	void CommandScriptHost::onClick(Click& click)
	{
		if (click.inOnline() && ScriptHostUtil::canSafelyTakeIt())
		{
			ScriptHostUtil::takeIt();
		}
	}
}
