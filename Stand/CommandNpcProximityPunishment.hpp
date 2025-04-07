#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "AllEntitiesEveryTick.hpp"
#include "punishments.hpp"

namespace Stand
{
	class CommandNpcProximityPunishment : public CommandToggle
	{
	private:
		const punishment_t mask;

	public:
		explicit CommandNpcProximityPunishment(CommandList* const parent, const Punishment& p)
			: CommandToggle(parent, Label(p.name), {}, Label(p.help_text)), mask(p.mask)
		{
		}

		void onEnable(Click& click) final
		{
			AllEntitiesEveryTick::npc_proximity_punishments |= mask;
		}

		void onDisable(Click& click) final
		{
			AllEntitiesEveryTick::npc_proximity_punishments &= ~mask;
		}
	};
}
