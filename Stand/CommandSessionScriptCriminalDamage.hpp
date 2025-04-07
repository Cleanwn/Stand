#pragma once

#include "CommandSessionScript.hpp"

#include "CommandLambdaAction.hpp"
#include "natives.hpp"

#include "CommandSessionScriptCriminalDamageScore.hpp"

namespace Stand
{
	class CommandSessionScriptCriminalDamage : public CommandSessionScript
	{
	public:
		explicit CommandSessionScriptCriminalDamage(CommandList* const parent)
			: CommandSessionScript(parent, ATSTRINGHASH("am_criminal_damage"))
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
						*ScriptLocal(thread, LOCAL_CRIMDMG_SHDATA).as<int*>() = 3;
					}
				});
			}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
			this->createChild<CommandSessionScriptCriminalDamageScore>();
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
						*ScriptLocal(thread, LOCAL_CRIMDMG_SHDATA + LOCAL_SHDATA_WARMUP).as<int*>() = NETWORK::GET_NETWORK_TIME() - ScriptGlobal(GLOBAL_TUNABLE_BASE + GLOBAL_TUNABLE_WARMUP_CRIMINAL_DAMAGE).get<int>();
					}
				});
			}, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY);
		}
	};
}
