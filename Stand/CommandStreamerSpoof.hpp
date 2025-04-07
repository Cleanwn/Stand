#pragma once

#include "CommandToggle.hpp"

#include <vector>

#include "AbstractPlayer.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandStreamerSpoof : public CommandToggle
	{
	public:
		explicit CommandStreamerSpoof(CommandList* const parent)
			: CommandToggle(parent, LOC("STRMSPF"), { CMDNAME("streamerspoof") }, LOC("GEN_H_LOC"))
		{
		}

		void onChange(Click& click) final
		{
			const std::vector<AbstractPlayer> list = AbstractPlayer::listExcept(true);
			g_gui.players_discovered = false;
			if (m_on)
			{
				AbstractPlayer::streamer_spoof = 1;
				for (const AbstractPlayer p : list)
				{
					p.spoofName();
					p.forceCommandRecreationIfNotUser();
				}
				AbstractPlayer::streamer_spoof = 2;
			}
			else
			{
				AbstractPlayer::streamer_spoof = 0;
				for (const AbstractPlayer p : list)
				{
					p.unspoofName();
					p.forceCommandRecreationIfNotUser();
				}
			}
			for (const auto& p : AbstractPlayer::listAll())
			{
				p.updateNameOnBlip();
			}
		}
	};
}
