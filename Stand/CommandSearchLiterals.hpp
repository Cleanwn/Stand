#pragma once

#include "CommandSearch.hpp"

#include "CommandLink.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	// A gerneralised search reyling on the heuristic that only commands with literal menu names are viable results.
	template <bool subfolders>
	class CommandSearchLiterals : public CommandSearch
	{
	public:
		explicit CommandSearchLiterals(CommandList* const parent, std::vector<CommandName>&& command_names)
			: CommandSearch(parent, LOC("SRCH"), std::move(command_names), NOLABEL, SEARCH_SIMPLIFIED)
		{
		}

		void doSearch(std::string&& arg) final
		{
			recursivelySearch(parent, arg);
		}

	protected:
		void recursivelySearch(CommandList* list, const std::string& arg)
		{
			for (const auto& child : list->children)
			{
				if (!child->getMenuName().isLiteral())
				{
					continue;
				}
				if (subfolders && child->isList())
				{
					recursivelySearch(child->as<CommandList>(), arg);
				}
				else
				{
					if (StringUtils::simplifyCommandName(child->getMenuName().getLiteralUtf8()).find(arg) != std::string::npos)
					{
						createChild<CommandLink>(child.get());
					}
				}
			}
		}
	};
}