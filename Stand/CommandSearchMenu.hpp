#pragma once

#include "CommandSearch.hpp"

namespace Stand
{
	class CommandSearchMenu : public CommandSearch
	{
	public:
		explicit CommandSearchMenu(CommandList* parent);

		void doSearch(std::string&& arg) final;
	private:
		void recursivelySearch(const std::string& arg, bool no_hiding, CommandList* list);
	};
}
