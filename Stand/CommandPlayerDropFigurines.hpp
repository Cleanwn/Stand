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
	class CommandPlayerDropFigurines : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerDropFigurines(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PLYCFIG"), { CMDNAME("figurines") }, LOC("PLYCFIG_H"), COMMANDPERM_NEUTRAL)
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
				else if(click.isWeb())
				{
					updateWebState();
				}
			}
			else if(!job_queued)
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
							if (STREAMING::HAS_MODEL_LOADED(ATSTRINGHASH("vw_prop_vw_colle_prbubble")))
							{
								PickupManager::CREATE_AMBIENT_PICKUP(ATSTRINGHASH("PICKUP_PORTABLE_CRATE_UNFIXED_INCAR_SMALL"), pos.x, pos.y, pos.z + 2.0f, 5, pickup_id, ATSTRINGHASH("vw_prop_vw_colle_prbubble"), FALSE, FALSE);
								if (++pickup_id > 99)
								{
									pickup_id = 0;
								}
							}
							else
							{
								STREAMING::REQUEST_MODEL(ATSTRINGHASH("vw_prop_vw_colle_prbubble"));
							}
							Script::current()->yield(10);
						}
					}
				});
			});
		}
	};
}
