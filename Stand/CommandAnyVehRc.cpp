#include "CommandAnyVehRc.hpp"

#include <soup/macros.hpp>
#include "script_func.hpp"

namespace Stand
{
	CommandAnyVehRc::CommandAnyVehRc(CommandList* const parent)
		: CommandToggle(parent, LOC("RCANYVEH"), CMDNAMES("rcanyvehicle"))
	{
	}

	void CommandAnyVehRc::onEnable(Click& click)
	{
		is_enabled = true;
		registerScriptTickEventHandler(click, [this]
		{
			if (m_on)
			{
				freemode_patch.onTick(ATSTRINGHASH("freemode"), FUNC_FM_ISRCVEH, ScriptFuncPatch::patchBytesReturnTrue_1, COUNT(ScriptFuncPatch::patchBytesReturnTrue_1));
				am_mp_rc_vehicle_patch.onTick(ATSTRINGHASH("am_mp_rc_vehicle"), FUNC_RC_ISRCPV, ScriptFuncPatch::patchBytesReturnTrue_1, COUNT(ScriptFuncPatch::patchBytesReturnTrue_1));
				return true;
			}
			freemode_patch.onDisable(ATSTRINGHASH("freemode"));
			am_mp_rc_vehicle_patch.onDisable(ATSTRINGHASH("am_mp_rc_vehicle"));
			is_enabled = false;
			return false;
		});
	}
}
