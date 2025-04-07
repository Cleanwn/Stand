#pragma once

#include "CommandActionScript.hpp"

namespace Stand
{
	class CommandPlayerBefriend : public CommandActionScript
	{
	public:
		explicit CommandPlayerBefriend(CommandList* const parent);

		void onClickScriptThread(Click& click) final;
	};
}
