#pragma once

#include "CommandSessionScript.hpp"

#include "CommandLambdaAction.hpp"

namespace Stand
{
	class CommandSessionScriptBb2 : public CommandSessionScript
	{
	public:
		explicit CommandSessionScriptBb2(CommandList* const parent)
			: CommandSessionScript(parent, ATSTRINGHASH("fm_content_business_battles"))
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
						*ScriptLocal(thread, LOCAL_BB2_STATE).as<int*>() = 3;
					}
				});
			}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
		}
	};
}
