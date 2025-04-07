#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandNoRollCooldown : public CommandToggle
	{
	private:
		soup::Bytepatch patch;

	public:
		explicit CommandNoRollCooldown(CommandList* const parent)
			: CommandToggle(parent, LOC("NOROLLCL"), combineCommandNames(CMDNAMES_OBF("no", "bypass", "disable"), CMDNAMES_OBF("rollcooldown", "combatrollcooldown")))
		{
		}

		void onEnable(Click& click) final
		{
			patch.initPatchNOP(pointers::CPedIntelligence_GetCanCombatRoll_patch, 9);
		}

		void onDisable(Click& click) final
		{
			patch.restore();
		}
	};
}