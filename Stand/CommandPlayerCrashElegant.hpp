#pragma once

#include "CommandPlayerCrash.hpp"

namespace Stand
{
	class CommandPlayerCrashElegant : public CommandPlayerCrash
	{
	public:
		explicit CommandPlayerCrashElegant(CommandList* const parent)
			: CommandPlayerCrash(parent, LOC("PLYCRSH_E"), CMDNAMES_OBF("crash"), LOC("PLYKCK_H_B_M"))
		{
		}

		void expandDong(std::vector<AbstractPlayer>&& targets) const final // TC_SCRIPT_YIELDABLE
		{
			for (const auto& p : targets)
			{
				p.crash();
			}
		}
	};
}
