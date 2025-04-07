#include "CommandFullPlayerList.hpp"

#include <soup/macros.hpp>

#include "AbstractPlayer.hpp"
#include "script_func.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	CommandFullPlayerList::CommandFullPlayerList(CommandList* const parent)
		: CommandToggle(parent, LOC("FLLPLYLST"), CMDNAMES_OBF("fullplayerlist"), LOC("FLLPLYLST_H"))
	{
	}

	void CommandFullPlayerList::onEnable(Click& click)
	{
		registerScriptTickEventHandler(click, [this]
		{
			if (m_on)
			{
				patch_1.onTick(ATSTRINGHASH("freemode"), FUNC_FM_LDB_ISLAND_CHK, ScriptFuncPatch::patchBytesReturnTrue_1, COUNT(ScriptFuncPatch::patchBytesReturnTrue_1));
				patch_2.onTick(ATSTRINGHASH("freemode"), FUNC_FM_SET_PLY_CNT, ScriptFuncPatch::patchBytesReturnVoid_1, COUNT(ScriptFuncPatch::patchBytesReturnVoid_1));
				ScriptGlobal(GLOBAL_LBD_PLAYER_COUNT).set<int>(AbstractPlayer::getPlayerCount());
				return true;
			}
			patch_1.onDisable(ATSTRINGHASH("freemode"));
			patch_2.onDisable(ATSTRINGHASH("freemode"));
			return false;
		});
	}
}
