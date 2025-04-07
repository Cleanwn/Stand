#pragma once

#include "CommandListSelectParticle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandDeathParticle : public CommandListSelectParticle
	{
	private:
		bool sent_fx = false;

	public:
		explicit CommandDeathParticle(CommandList* const parent)
			: CommandListSelectParticle(parent, LOC("DEDFX"), NOLABEL, INCLUDE_NONE, 0)
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
					if (!sent_fx)
					{
						Particles::tryPlay(this->value, g_player_ped.getPos());
						sent_fx = true;
					}
				}
				else
				{
					sent_fx = false;
				}

				return this->value == cvalue;
			});
		}
	};
}
