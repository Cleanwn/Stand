#pragma once

#include "CommandSessionScript.hpp"

#include "CommandLambdaAction.hpp"

namespace Stand
{
	class CommandSessionScriptBeast : public CommandSessionScript
	{
	public:
		explicit CommandSessionScriptBeast(CommandList* const parent)
			: CommandSessionScript(parent, ATSTRINGHASH("am_hunt_the_beast"))
		{
			this->createChild<CommandLambdaAction>(LOC("LUA_STOP"), {}, NOLABEL, [this](Click& click)
			{
				if (!click.isUltimateEdition())
				{
					return;
				}
				click.ensureYieldableScriptThread([this]
				{
					if (auto thread = getThreadAndEnsureControl())
					{
						*ScriptLocal(thread, LOCAL_HTB_SHDATA).as<int*>() = 4;
					}
				});
			}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
			this->createChild<CommandLambdaAction>(LOC("STRTINST"), {}, NOLABEL, [this](Click& click)
			{
				if (!click.isUltimateEdition())
				{
					return;
				}
				click.ensureYieldableScriptThread([this]
				{
					if (auto thread = getThreadAndEnsureControl())
					{
						*ScriptLocal(thread, LOCAL_HTB_SHDATA + 1 + 4).as<int*>() = 0;
					}
				});
			}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
		}
	};
}
