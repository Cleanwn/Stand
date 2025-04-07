#pragma once

#include "CommandActionScript.hpp"

#include "StringCastable.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandVector3Copy : public CommandActionScript
	{
	public:
		explicit CommandVector3Copy(CommandList* const parent, std::vector<CommandName>&& command_names)
			: CommandActionScript(parent, LOC("CPPOS"), std::move(command_names))
		{
		}

		void onClickScriptThread(Click& click) final
		{
			auto vec = parent->as<CommandVector3>()->getVec();
			Util::copy_to_clipboard(click, vec);
		}
	};
}
