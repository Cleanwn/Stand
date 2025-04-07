#pragma once

#define ENABLE_LEFT_DUE_TO_CHEATING false

#if ENABLE_LEFT_DUE_TO_CHEATING
#include "CommandTextslider.hpp"
#else
#include "CommandAction.hpp"
#endif

#include "TransitionHelper.hpp"

namespace Stand
{
#if ENABLE_LEFT_DUE_TO_CHEATING
	using CommandBeAloneBase = CommandTextslider;
#else
	using CommandBeAloneBase = CommandAction;
#endif

	class CommandBeAlone : public CommandBeAloneBase
	{
	public:
		explicit CommandBeAlone(CommandList* const parent)
			: CommandBeAloneBase(parent, LOC("SLFDS"), { CMDNAME("bealone"), CMDNAME("lagout") }, LOC("SLFDS_H")
#if ENABLE_LEFT_DUE_TO_CHEATING
				, {
				{ -1, LOC("SLR_N") },
				{ /* BAIL_CONSOLE_BAN */ 19, LOC("SLR_C") },
			}
#endif
			)
		{
		}

		void onClick(Click& click) final
		{
			if (click.inOnline())
			{
				auto other_players = AbstractPlayer::listExcept(true);
				if (other_players.empty())
				{
					click.uwotm8();
				}
				else
				{
					TransitionHelper::beAlone(other_players
#if ENABLE_LEFT_DUE_TO_CHEATING
						, (int)this->value
#endif
					);
				}
			}
		}
	};
}
