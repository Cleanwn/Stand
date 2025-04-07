#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandActionScript.hpp"

namespace Stand
{
	class CommandPlayerAmmo : public CommandOnPlayer<CommandActionScript>
	{
	public:
		explicit CommandPlayerAmmo(CommandList* const parent);

		void onClickScriptThread(Click& click) final;
	};
}
