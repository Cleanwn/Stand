#include "CommandNoDailyExpenses.hpp"

#include <soup/macros.hpp>
#include "script_func.hpp"
#include "ScriptVmOpCode.hpp"

namespace Stand
{
	CommandNoDailyExpenses::CommandNoDailyExpenses(CommandList* parent)
		: CommandToggle(parent, LOC("NDLYXPS"), { CMDNAME("nodailyexpenses") })
	{
	}

	void CommandNoDailyExpenses::onEnable(Click& click)
	{
		registerScriptTickEventHandler(click, [this]
		{
			if (m_on)
			{
				patch.onTick(ATSTRINGHASH("freemode"), FUNC_FM_DAILY_EXPENSES, ScriptFuncPatch::patchBytesReturnVoid_0, COUNT(ScriptFuncPatch::patchBytesReturnVoid_0));
				return true;
			}
			patch.onDisable(ATSTRINGHASH("freemode"));
			return false;
		});
	}
}
