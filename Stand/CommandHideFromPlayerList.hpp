#pragma once

#include "CommandToggle.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandHideFromPlayerList : public CommandToggle
	{
	public:
		explicit CommandHideFromPlayerList(CommandList* const parent)
			: CommandToggle(parent, LOC("HIDEPLIST"), CMDNAMES_OBF("hidefromplayerlist"), LOC("HIDEPLIST_H"))
		{
		}

		void onEnable(Click& click) final
		{
			g_hooking.hide_from_player_list = true;
		}

		void onDisable(Click& click) final
		{
			g_hooking.hide_from_player_list = false;
			ensureScriptThread([]
			{
				// Since we're in a script thread, og_cayo_flags will have been written to the script global.
				// The script global won't be tampered with on post-tick because the bool is false now.
				// So we just have to stop locking the flags.
				g_hooking.og_cayo_flags.reset();
			});
		}
	};
}
