#pragma once

#include "CommandIssuable.hpp"

#include "get_next_arg.hpp"
#include "Script.hpp"
#include "str2int.hpp"

namespace Stand
{
	class CommandWait : public CommandIssuable
	{
	public:
		explicit CommandWait(CommandList* parent)
			: CommandIssuable(parent, { CMDNAME("wait"), CMDNAME("sleep"), CMDNAME("delay") })
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto arg = get_next_arg(args);
			auto it = &*arg.cbegin();
			auto ms = str2int_it<time_t>(it);
			if (*--it == 's')
			{
				ms *= 1000;
			}
			Script::current()->yield(ms);
		}
	};
}
