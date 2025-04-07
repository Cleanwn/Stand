#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandSpoofName : public CommandToggle
	{
	public:
		explicit CommandSpoofName(CommandList* const parent)
			: CommandToggle(parent, LOC("SPFNME_D"), { CMDNAME("spoofname") }, LOC("GEN_H_LOC"))
		{
		}

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
