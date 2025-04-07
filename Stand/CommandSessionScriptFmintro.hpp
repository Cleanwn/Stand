#pragma once

#include "CommandSessionScript.hpp"

#include "CommandLambdaAction.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandSessionScriptFmintro : public CommandSessionScript
	{
	public:
		explicit CommandSessionScriptFmintro(CommandList* const parent)
			: CommandSessionScript(parent, ATSTRINGHASH("FM_INTRO"))
		{
			this->createChild<CommandLambdaAction>(LOC("LUA_STOP"), {}, NOLABEL, [this](Click& click)
			{
				click.ensureYieldableScriptThread([this]
				{
					if (auto thread = getThreadAndEnsureControl())
					{
						*ScriptLocal(thread, 536).as<int*>() = 6;
					}
				});
			}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
		}
	};
}
