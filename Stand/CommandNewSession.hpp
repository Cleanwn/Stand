#pragma once

#include "CommandListNameshare.hpp"

#include "SessionType.hpp"

namespace Stand
{
	class CommandNewSession : public CommandListNameshare
	{
	private:
		CommandToggleNoCorrelation* select_character;

	public:
		explicit CommandNewSession(CommandList* const parent);

		void onChildClick(Click& click, SessionType value);
	};
}
