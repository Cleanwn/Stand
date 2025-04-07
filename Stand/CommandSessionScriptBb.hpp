#pragma once

#include "CommandSessionScript.hpp"

#include "CommandLambdaAction.hpp"
#include "ScriptLocal.hpp"

namespace Stand
{
	class CommandSessionScriptBb : public CommandSessionScript
	{
	public:
		explicit CommandSessionScriptBb(CommandList* const parent)
			: CommandSessionScript(parent, ATSTRINGHASH("business_battles"))
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
						*ScriptLocal(thread, LOCAL_BB_SHDATA + 958).as<int*>() = 3;
					}
				});
			}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
		}
	};
}
