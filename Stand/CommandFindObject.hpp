#pragma once

#include "CommandSearch.hpp"

#include "StringUtils.hpp"

namespace Stand
{
	class CommandFindObject : public CommandSearch
	{
	public:
		explicit CommandFindObject(CommandList* parent)
			: CommandSearch(parent, LOC("SRCH"), { CMDNAME("findobject") })
		{
		}

		void onClick(Click& click) final
		{
			if (click.isRegularEdition())
			{
				CommandSearch::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (click.isRegularEdition())
			{
				CommandSearch::onCommand(click, args);
			}
		}

		void doSearch(std::string&& arg) final
		{
			StringUtils::simplifyCommandName(arg);
			for (auto i = parent->children.cbegin() + ((CommandObjectSpawner*)parent)->objects_offset; i != parent->children.cend(); ++i)
			{
				if ((*i)->isList())
				{
					for (const auto& cmd : ((CommandList*)i->get())->children)
					{
						checkForMatchByMenuName(arg, (CommandPhysical*)cmd.get());
					}
				}
				else
				{
					checkForMatchByMenuName(arg, (CommandPhysical*)i->get());
				}
			}
			sortChildren();
		}
	};
}
