#pragma once

#include "CommandBoneEsp.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandNpcBoneEsp : public CommandBoneEsp
	{
	private:
		CommandToggleNoCorrelation* const exclude_dead;

	public:
		explicit CommandNpcBoneEsp(CommandList* const parent, CommandToggleNoCorrelation* exclude_dead, CommandColour* const colour)
			: CommandBoneEsp(parent, { CMDNAME("npcboneesp"), CMDNAME("npcbonesp") }, colour), exclude_dead(exclude_dead)
		{
		}

	protected:
		bool filterPed(AbstractEntity& ped) const final
		{
			return !ped.isAPlayer()
				&& (!exclude_dead->m_on || !ped.isDead());
		}
	};
}
