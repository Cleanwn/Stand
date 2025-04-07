#pragma once

#include "CommandToggleRegular.hpp"

namespace Stand
{
	class CommandDesyncKickKarma : public CommandToggleRegular
	{
	public:
		explicit CommandDesyncKickKarma(CommandList* const parent)
			: CommandToggleRegular(parent, LOC("DSKK"), { CMDNAME("desynckarma") }, LOC("DSKK_H2"))
		{
		}

		void onChangeInner(Click& click) final
		{
			CommandPlayer::desync_kick_karma = m_on;
		}
	};
}
