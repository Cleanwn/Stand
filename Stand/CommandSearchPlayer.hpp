#pragma once

#include "CommandSearch.hpp"

namespace Stand
{
	class CommandSearchPlayer : public CommandSearch
	{
	public:
		explicit CommandSearchPlayer(CommandList* parent);

		void doSearch(std::string&& arg) final;
	};
}
