#pragma once

#include "CommandListSelectParticle.hpp"

#include "AbstractEntity.hpp"
#include "FiberPool.hpp"
#include "is_session.hpp"

namespace Stand
{
	class CommandRespawnParticle : public CommandListSelectParticle
	{
	private:
		bool dead_last_tick = false;

	public:
		explicit CommandRespawnParticle(CommandList* const parent)
			: CommandListSelectParticle(parent, LOC("RESPFX"), NOLABEL, INCLUDE_NONE, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			const auto cvalue = this->value;
			registerScriptTickEventHandler([this, cvalue]
			{
				if (this->value == 0)
				{
					return false;
				}
				
				if (g_player_ped.isDead())
				{
					dead_last_tick = true;
				}
				else if (dead_last_tick)
				{
					FiberPool::queueJob([=]
					{
						Particles::play(this->value, g_player_ped.getPos());
					});
					dead_last_tick = false;
				}

				return this->value == cvalue;
			});
		}
	};
}
