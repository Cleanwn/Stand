#pragma once

#include "CommandList.hpp"

#include <fmt/core.h>

#include "CommandAction.hpp"

namespace Stand
{
	class CommandListMillion : public CommandList
	{
	public:
		explicit CommandListMillion(CommandList* const parent)
			: CommandList(parent, LIT_OBF("A Million Things"), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_TEMPORARY)
		{
		}

		void onClick(Click& click) final
		{
			this->children.reserve(1000000);
			for (cursor_t i = 1; i <= 1000000; ++i)
			{
				this->children.emplace_back(std::make_unique<CommandAction>(this, LIT(fmt::to_string(i)), CMDNAMES(), NOLABEL, CMDFLAG_TEMPORARY));
			}
			CommandList::onClick(click);
		}

		void onBack(ThreadContext thread_context) final
		{
			resetChildren();
		}
	};
}
