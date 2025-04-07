#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "AllEntitiesEveryTick.hpp"
#include "punishments.hpp"

namespace Stand
{
	class CommandExistencePunishment : public CommandToggle
	{
	private:
		const punishment_t mask;

	public:
		explicit CommandExistencePunishment(CommandList* const parent, const Punishment& p)
			: CommandToggle(parent, Label(p.name), {}, Label(p.help_text)), mask(p.mask)
		{
			switch (p.mask)
			{
			case PUNISHMENT_UNARM:
			case PUNISHMENT_DELETE:
				flags &= ~CMDFLAG_SUPPORTS_STATE_OPERATIONS;
				break;
			}
		}

		void onEnable(Click& click) final
		{
			AllEntitiesEveryTick::npc_existence_punishments |= mask;
		}

		void onDisable(Click& click) final
		{
			AllEntitiesEveryTick::npc_existence_punishments &= ~mask;
		}
	};
}
