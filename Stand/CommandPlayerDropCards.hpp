#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "FiberPool.hpp"
#include "atStringHash.hpp"
#include "natives.hpp"
#include "PickupManager.hpp"
#include "PlayerProvider.hpp"
#include "Script.hpp"

namespace Stand
{
	class CommandPlayerDropCards : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerDropCards(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PLYCRDS"), { CMDNAME("cards") }, LOC("PLYCRDS_H"), COMMANDPERM_NEUTRAL)
		{
		}

		void onClick(Click& click) final
		{
			if (!m_on)
			{
				if (click.inOnline())
				{
					if (!job_queued)
					{
						m_on = !m_on;
						updateState(click);
						onEnable(click);
					}
				}
				else if (click.isWeb())
				{
					updateWebState();
				}
			}
			else if (!job_queued)
			{
				m_on = !m_on;
				updateState(click);
			}
		}

		void onEnable(Click& click) final
		{
			FiberPool::queueJob([this]
			{
				PP_PTR->toggleFiberBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [](const std::vector<AbstractPlayer>& players)
				{
					static int pickup_id = 0;
					for (const AbstractPlayer p : players)
					{
						if (p.hasPed())
						{
							auto pos = p.getPos();
							if (STREAMING::HAS_MODEL_LOADED(ATSTRINGHASH("vw_prop_vw_lux_card_01a")))
							{
								PickupManager::CREATE_AMBIENT_PICKUP(ATSTRINGHASH("PICKUP_PORTABLE_CRATE_UNFIXED_INCAR_SMALL"), pos.x, pos.y, pos.z + 2.0f, 5, pickup_id, ATSTRINGHASH("vw_prop_vw_lux_card_01a"), FALSE, FALSE);
								if (++pickup_id > 53)
								{
									pickup_id = 0;
								}
							}
							else
							{
								STREAMING::REQUEST_MODEL(ATSTRINGHASH("vw_prop_vw_lux_card_01a"));
							}
							Script::current()->yield(10);
						}
					}
				});
			});
		}
	};
}
