#pragma once

#include "CommandListWithOnTickAsActiveList.hpp"
#include "CommandListConcealer.hpp"

namespace Stand
{
	class CommandListHandling : public CommandListWithOnTickAsActiveList<CommandListConcealer>
	{
	private:
		CommandToggle* show_na;

	public:
		explicit CommandListHandling(CommandList* const parent);

		void onTickAsActiveList() final;
	};

	class CommandListSubhandling;
}
