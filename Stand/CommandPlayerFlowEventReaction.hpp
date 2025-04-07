#pragma once

#include "CommandToggle.hpp"

#include "CommandListPlayer.hpp"
#include "Hooking.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	class CommandPlayerFlowEventReaction : public CommandToggle
	{
	private:
		floweventreaction_t* const reactions;
		const floweventreaction_t reaction;

	public:
		explicit CommandPlayerFlowEventReaction(CommandList* const parent, floweventreaction_t* const reactions, const floweventreaction_t reaction, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), reactions(reactions), reaction(reaction)
		{
			PC_PTR->registerCommand(this);
		}

		void onChange(Click& click) final
		{
			if (m_on)
			{
				DEF_P2;
				if (pp->single && pp->getPlayers(true).at(0) == g_player)
				{
					if (click.type != CLICK_AUTO)
					{
						click.setResponse(LOC("CMDOTH"));
					}
					setOnIndication(false);
					return;
				}
			}
			if (m_on)
			{
				*reactions |= reaction;
			}
			else
			{
				*reactions &= ~reaction;
			}
		}
	};
}
