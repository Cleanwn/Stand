#pragma once

#include "CommandPlayerToggle.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "FiberPool.hpp"
#include "atStringHash.hpp"
#include "natives.hpp"
#include "PickupManager.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	class CommandPlayerSnack : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerSnack(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PLYSNK"), { CMDNAME("snack") }, NOLABEL, COMMANDPERM_NEUTRAL)
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
							AbstractEntity ped = p.getPed();
							const v3 pos = ped.getPos();
							if (pos.z != -50.0f)
							{
								if (STREAMING::HAS_MODEL_LOADED(ATSTRINGHASH("prop_choc_pq")))
								{
									PickupManager::CREATE_AMBIENT_PICKUP(ATSTRINGHASH("pickup_health_snack"), pos.x, pos.y, pos.z, 5, pickup_id, ATSTRINGHASH("prop_choc_pq"), false, false);
									if (++pickup_id > 99)
									{
										pickup_id = 0;
									}
								}
								else
								{
									STREAMING::REQUEST_MODEL(ATSTRINGHASH("prop_choc_pq"));
								}
								Script::current()->yield(10);
							}
						}
					}
				});
			});
		}
	};
}
