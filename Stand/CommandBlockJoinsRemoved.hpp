#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandBlockJoinsRemoved : public CommandToggle
	{
	public:
		explicit CommandBlockJoinsRemoved(CommandList* const parent)
			: CommandToggle(parent, LOC("BJ_REM"), {}, LOC("BJ_REM_H"))
		{
		}

		Label getActivationName() const final
		{
			return LOC("BJ_REM_A");
		}

		void onChange(Click& click) final
		{
			g_hooking.block_joins_from_removed_players = m_on;
		}
	};
}
