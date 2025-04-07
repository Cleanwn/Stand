#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListStarred : public CommandList
	{
	public:
		explicit CommandListStarred(CommandList* parent);

		void onActiveListUpdate() final;
	private:
		void recursivelyPopulate(CommandList* list, const std::string& path_prefix);
	};
}
