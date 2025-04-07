#pragma once

#include "CommandToggle.hpp"

#include <soup/Bytepatch.hpp>

#include "pointers.hpp"

namespace Stand
{
	class CommandPreserveWalkStyleDuringCombat : public CommandToggle
	{
	private:
		soup::Bytepatch patch;

	public:
		explicit CommandPreserveWalkStyleDuringCombat(CommandList* const parent)
			: CommandToggle(parent, LOC("PRSVRWALK"), CMDNAMES_OBF("preservewalkstyle"), LOC("PRSVRWALK_H"))
		{
		}

		void onEnable(Click& click) final
		{
			uint8_t bytes[] = {
				0x31, 0xc0, // xor eax, eax
				soup::Bytepatch::RET
			};
			patch.initPatch(pointers::CPed_WantsToUseActionMode, bytes, COUNT(bytes));
		}

		void onDisable(Click& click) final
		{
			patch.restore();
		}
	};
}