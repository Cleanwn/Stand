#pragma once

#include "CommandSearch.hpp"

#include "StringUtils.hpp"

namespace Stand
{
	class CommandFindModel : public CommandSearch
	{
	public:
		explicit CommandFindModel(CommandList* const parent)
			: CommandSearch(parent, LOC("SRCH"), CMDNAMES("findmodel"), LOC("FNDMDL_H"))
		{
		}

		void doSearch(std::string&& arg) final
		{
			StringUtils::simplifyCommandName(arg);
			for (const auto& child : parent->children)
			{
				if (!child->isList())
				{
					continue;
				}
				for (auto& _model : ((const CommandList*)child.get())->children)
				{
					checkForMatchByMenuName(arg, (CommandPhysical*)_model.get());
				}
			}
		}
	};
}
