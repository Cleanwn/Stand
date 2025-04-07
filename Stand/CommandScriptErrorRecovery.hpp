#pragma once

#include "CommandListSelect.hpp"

#include "ScriptErrorRecoveryMode.hpp"

namespace Stand
{
	class CommandScriptErrorRecovery : public CommandListSelect
	{
	public:
		explicit CommandScriptErrorRecovery(CommandList* const parent)
			: CommandListSelect(parent, LOC("SVM"), { CMDNAME("scripterrorrecovery") }, NOLABEL, {
				{ScriptErrorRecoveryMode::DISABLED, LOC("DOFF"), CMDNAMES("off")},
				{ScriptErrorRecoveryMode::FREEMODE_ONLY, LOC("4FMONLY"), CMDNAMES("freemodeonly")},
				{ScriptErrorRecoveryMode::ENABLED, LOC("ON"), CMDNAMES("on")},
			}, ScriptErrorRecoveryMode::ENABLED)
		{
		}
		
		void onChange(Click& click, long long prev_value) final
		{
			script_error_recovery_mode = (ScriptErrorRecoveryMode_t)value;
		}
	};
}
