#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandActionScript.hpp"

namespace Stand
{
	class CommandPlayerCopyOutfit : public CommandOnPlayer<CommandActionScript>
	{
	public:
		explicit CommandPlayerCopyOutfit(CommandList* const parent);

		void onClickScriptThread(Click& click) final;
	};
}
