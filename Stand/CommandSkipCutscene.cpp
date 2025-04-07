#include "CommandSkipCutscene.hpp"

#include <soup/Bytepatch.hpp>
#include <soup/macros.hpp>

#include "FiberPool.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "Script.hpp"

namespace Stand
{
	CommandSkipCutscene::CommandSkipCutscene(CommandList* const parent)
		: CommandAction(parent, LOC("SKPCTSCN"), { CMDNAME("skipcutscene") })
	{
	}

	static inline soup::Bytepatch get_bink_movie_time_pre_ret_patch{};

	void CommandSkipCutscene::onClick(Click& click)
	{
		ensureScriptThread(click, [this](ThreadContext thread_context)
		{
			CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
			if (get_bink_movie_time_pre_ret_patch.isPatched())
			{
				return;
			}
			uint8_t patch[] = {
				0x48, 0xc7, 0xc0, 0x64, 0x00, 0x00, 0x00, // mov    rax, 100
				0xf3, 0x48, 0x0f, 0x2a, 0xc0,             // cvtsi2ss xmm0, rax
				0xc3,                                     // ret
			};
			get_bink_movie_time_pre_ret_patch.initPatch(pointers::get_bink_movie_time_pre_ret, patch, COUNT(patch));
			ensureYieldableScriptThread(thread_context, []
			{
				for (int i = 0; i < 7; i++)
				{
					Script::current()->yield();
				}
				get_bink_movie_time_pre_ret_patch.restore();
			});
		});
	}
}
