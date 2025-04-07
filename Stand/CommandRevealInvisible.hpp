#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandRevealInvisible : public CommandToggle
	{
	public:
		explicit CommandRevealInvisible(CommandList* const parent)
			: CommandToggle(parent, LOC("REVINV"), CMDNAMES_OBF("revealinvisible"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					NETWORK::SET_PLAYER_VISIBLE_LOCALLY(p, TRUE);
				}
			});
		}
	};
}
