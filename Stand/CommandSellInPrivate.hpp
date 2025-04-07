#pragma once

#include "CommandToggle.hpp"

#include "ScriptGlobal.hpp"
#include "regular_event.hpp"

namespace Stand
{
	class CommandSellInPrivate : public CommandToggle
	{
	public:
		explicit CommandSellInPrivate(CommandList* const parent)
			: CommandToggle(parent, LOC("SLLINPRV"), { CMDNAME("sellinprivate") })
		{
		}

		void onChange(Click& click) final
		{
			registerScriptTickEventHandler(click, [=]()
			{
				const bool is_solo = NETWORK::NETWORK_SESSION_IS_SOLO();
				if (m_on)
				{
					if (*pointers::is_session_started)
					{
						if (is_solo)
						{
							ScriptGlobal(GLOBAL_CURRENT_SESSION_TYPE).set<BOOL>(0);
						}
						ScriptGlobal(GLOBAL_PRIVATE_SESSION).set<BOOL>(false);
					}
					return true;
				}
				else
				{
					if (is_solo)
					{
						ScriptGlobal(GLOBAL_CURRENT_SESSION_TYPE).set<BOOL>(10);
					}
					ScriptGlobal(GLOBAL_PRIVATE_SESSION).set<BOOL>(is_solo || NETWORK::NETWORK_SESSION_GET_PRIVATE_SLOTS() > 0 || NETWORK::NETWORK_SESSION_IS_CLOSED_FRIENDS() || NETWORK::NETWORK_SESSION_IS_CLOSED_CREW() || NETWORK::NETWORK_SESSION_IS_PRIVATE());
					return false;
				}
			});
		}
	};
}
