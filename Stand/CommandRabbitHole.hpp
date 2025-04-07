#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandRabbitHole : public CommandList
	{
	public:
		explicit CommandRabbitHole(CommandList* const parent)
			: CommandList(parent, LIT_OBF("The Rabbit Hole"), CMDNAMES(), NOLABEL, CMDFLAGS_LIST | CMDFLAG_TEMPORARY)
		{
		}

		void onClick(Click& click) final
		{
			if (children.empty())
			{
				createChild<CommandRabbitHole>();
			}
			CommandList::onClick(click);
		}
	};
}
