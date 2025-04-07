#pragma once

#include "CommandToggleRegular.hpp"

#include "ComponentNetcode.hpp"

namespace Stand
{
	class CommandHostKickKarma : public CommandToggleRegular
	{
	public:
		explicit CommandHostKickKarma(CommandList* const parent)
			: CommandToggleRegular(parent, LOC("PTX_HKK"), {}, LOC("PTX_HKK_H"))
		{
		}

		void onChangeInner(Click& click) final
		{
			g_comp_netcode.host_kick_karma = m_on;
		}
	};
}
