#pragma once

#include "CommandToggle.hpp"

#include <soup/macros.hpp>
#include "script_func.hpp"
#include "ScriptFuncPatch.hpp"

namespace Stand
{
	class CommandTogglableBigmap : public CommandToggle
	{
	private:
		ScriptFuncPatch patch;

	public:
		CommandTogglableBigmap(CommandList* const parent)
			: CommandToggle(parent, LOC("TGGLEMAP"), CMDNAMES("toggleablebigmap"), LOC("TGGLEMAP_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					patch.onTick(ATSTRINGHASH("freemode"), FUNC_FM_BIGMAP, ScriptFuncPatch::patchBytesNop3, COUNT(ScriptFuncPatch::patchBytesNop3), 79);
					return true;
				}
				patch.onDisable(ATSTRINGHASH("freemode"));
				return false;
			});
		}
	};
}
