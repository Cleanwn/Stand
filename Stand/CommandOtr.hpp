#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "is_session.hpp"

#include "drawDebugText.hpp"

namespace Stand
{
	class CommandOtr : public CommandToggle
	{
	public:
		explicit CommandOtr(CommandList* const parent)
			: CommandToggle(parent, LOC("OTR"), { CMDNAME("otr"), CMDNAME("offtheradar"), CMDNAME("offradar") })
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]()
			{
				if (is_session_started_and_transition_finished())
				{
					*ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(g_player, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_OTR1).as<BOOL*>() = m_on;
					if (m_on)
					{
						*ScriptGlobal(GLOBAL_OTRDEADLINE1).as<int*>() = NETWORK::GET_NETWORK_TIME() - (60000 - 42000);
						*ScriptGlobal(GLOBAL_OTRDEADLINE2).as<int*>() = 0x7FFFFFFF;
					}
				}
				return m_on;
			});
		}
	};
}
