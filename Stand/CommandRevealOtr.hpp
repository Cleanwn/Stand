#pragma once

#include "CommandToggle.hpp"

#include "ScriptFuncPatch.hpp"

namespace Stand
{
	class CommandRevealOtr : public CommandToggle
	{
	private:
		ScriptFuncPatch patch_1;
		ScriptFuncPatch patch_2;

	public:
		explicit CommandRevealOtr(CommandList* const parent)
			: CommandToggle(parent, LOC("REVOTR"), CMDNAMES_OBF("revealotr"))
		{
		}

		void onEnable(Click& click) final
		{
			registerAsyncTickEventHandler(click, [this]()
			{
				if (m_on)
				{
					// SHOULD_BLOCK_PLAYER_BLIP
					patch_1.onTick(ATSTRINGHASH("freemode"), FUNC_FM_SHOULDHIDEBLIP, ScriptFuncPatch::patchBytesReturnFalse_1, COUNT(ScriptFuncPatch::patchBytesReturnFalse_1));

					// SHOULD_BLIP_PLAYER
					patch_2.onTick(ATSTRINGHASH("freemode"), FUNC_FM_SHOULDSHOWBLIP, ScriptFuncPatch::patchBytesReturnTrue_1, COUNT(ScriptFuncPatch::patchBytesReturnTrue_1));

					//// SHOULD_HIDE_DEAD_PLAYER_BLIPS (1.61 func_14256)
					//*ScriptGlobal(2678393 + 1658).as<int*>() = SCRIPT::GET_ID_OF_THIS_THREAD();
					//*ScriptGlobal(2678393 + 1659).as<BOOL*>() = FALSE;

					// HANDLE_DEAD_PLAYER_BLIPS: IF NOT IS_BIT_SET(g_PlayerBlipsData.bs_FadeOutDeadBlip, g_PlayerBlipsData.iBlipStaggerCount)
					*ScriptGlobal(GLOBAL_FADEOUTDEADBLIP).as<int*>() |= (1 << ScriptGlobal(GLOBAL_BLIPSTAGGERCOUNT).get<int>());

					return true;
				}
				patch_1.onDisable(ATSTRINGHASH("freemode"));
				patch_2.onDisable(ATSTRINGHASH("freemode"));
				*ScriptGlobal(GLOBAL_FADEOUTDEADBLIP).as<int*>() &= ~(1 << ScriptGlobal(GLOBAL_BLIPSTAGGERCOUNT).get<int>());
				return false;
			});
		}
	};
}
