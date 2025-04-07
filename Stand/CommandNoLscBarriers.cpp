#include "CommandNoLscBarriers.hpp"

#include "script_func.hpp"

namespace Stand
{
	CommandNoLscBarriers::CommandNoLscBarriers(CommandList* parent)
		: CommandToggle(parent, LOC("NLSCBAR"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAMES("lscrestrictions")), LOC("NLSCBAR_H"))
	{
	}

	void CommandNoLscBarriers::onEnable(Click& click)
	{
		registerScriptTickEventHandler(click, [this]
		{
			if (m_on)
			{
				// Allow entering LSC with all vehicles.
				patch_1.onTick(ATSTRINGHASH("carmod_shop"), FUNC_CM_CANENTER, ScriptFuncPatch::patchBytesReturnTrue_3, COUNT(ScriptFuncPatch::patchBytesReturnTrue_3));

				// And allow full customisation, e.g. on Arena War vehicels.
				patch_2.onTick(ATSTRINGHASH("carmod_shop"), FUNC_CM_CANFULLYMOD, ScriptFuncPatch::patchBytesReturnTrue_3, COUNT(ScriptFuncPatch::patchBytesReturnTrue_3));

				return true;
			}
			patch_1.onDisable(ATSTRINGHASH("carmod_shop"));
			patch_2.onDisable(ATSTRINGHASH("carmod_shop"));
			return false;
		});
	}
}
