#pragma once

#include "CommandActionScript.hpp"

namespace Stand
{
	class CommandPlayerProfile : public CommandActionScript
	{
	public:
		explicit CommandPlayerProfile(CommandList* const parent);

		void onClickScriptThread(Click& click) final;
	};
}
