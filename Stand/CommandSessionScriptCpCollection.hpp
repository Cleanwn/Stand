#pragma once

#include "CommandSessionScript.hpp"

#include "CommandLambdaAction.hpp"

namespace Stand
{
	class CommandSessionScriptCpCollection : public CommandSessionScript
	{
	public:
		explicit CommandSessionScriptCpCollection(CommandList* const parent)
			: CommandSessionScript(parent, ATSTRINGHASH("AM_CP_COLLECTION"))
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
						*ScriptLocal(thread, LOCAL_CPCOL_SHDATA).as<int*>() = 3;
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
						*ScriptLocal(thread, LOCAL_CPCOL_SHDATA + 667).as<int*>() = 0;
					}
				});
			}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
		}
	};
}
