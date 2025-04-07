#pragma once

#include "Gui.hpp"

namespace Stand
{
	class CommandLuaConfigEnforceSilentStop : public CommandListSelect
	{
	public:
		explicit CommandLuaConfigEnforceSilentStop(CommandList* const parent)
			: CommandListSelect(parent, LOC("ENFRCSILNTSTP"), {}, NOLABEL, std::vector<CommandListActionItemData>{
				{0, LOC("NO")},
				{ 1, LOC("YES") },
				{ 2, LOC("YES_NOTIFY") },
			}, 1)
		{
		}

		void applyDefaultState() final
		{
			if (g_gui.canUpdateThemeSetting())
			{
				CommandListSelect::applyDefaultState();
			}
		}
	};
}
