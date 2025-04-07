#include "CommandBypassTimeTrialUnsuitable.hpp"

#include <soup/macros.hpp>
#include "script_func.hpp"

namespace Stand
{
	CommandBypassTimeTrialUnsuitable::CommandBypassTimeTrialUnsuitable(CommandList* parent)
		: CommandToggle(parent, LOC("AMTTSUIT"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAME("timetrialsuitability")))
	{
	}

	void CommandBypassTimeTrialUnsuitable::onEnable(Click& click)
	{
		registerScriptTickEventHandler(click, [this]
		{
			if (m_on)
			{
				patch.onTick(ATSTRINGHASH("freemode"), FUNC_FM_AMTT_NSUIT, ScriptFuncPatch::patchBytesReturnTrue_2, COUNT(ScriptFuncPatch::patchBytesReturnTrue_2));
				return true;
			}
			patch.onDisable(ATSTRINGHASH("freemode"));
			return false;
		});
	}
}
