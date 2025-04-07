#pragma once

#include "CommandToggle.hpp"

#include "PlayerListSort.hpp"

namespace Stand
{
	class CommandPlayerListSortReverse : public CommandToggle
	{
	public:
		explicit CommandPlayerListSortReverse(CommandList* const parent)
			: CommandToggle(parent, LOC("SRTREV"))
		{
		}

		void onChange(Click& click) final
		{
			PlayerListSort::desc = m_on;
			PlayerListSort::update();
		}
	};
}
