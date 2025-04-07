#pragma once

#include "CommandToggleUltimate.hpp"

#include "AbstractPlayer.hpp"
#include "evtHostChangeEvent.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandBlockJoinsAnyone : public CommandToggleUltimate
	{
	public:
		explicit CommandBlockJoinsAnyone(CommandList* const parent)
			: CommandToggleUltimate(parent, LOC("BJ_ALL"), CMDNAMES_OBF("blockjoins"))
		{
		}

		Label getActivationName() const final
		{
			return LOC("BJ_ALL_A");
		}

		static void onHostChange(evtHostChangeEvent& event)
		{
			if (g_player == event.cur)
			{
				NETWORK::NETWORK_SESSION_BLOCK_JOIN_REQUESTS(true);
			}
		}

		void onChangeInner(Click& click) final
		{
			g_hooking.block_joins_from_anyone = m_on;
			if (m_on)
			{
				evtHostChangeEvent::registerHandler(&onHostChange);
			}
			else
			{
				evtHostChangeEvent::unregisterHandler(&onHostChange);
			}
			if (g_player.isHost())
			{
				ensureScriptThread(click, [this]
				{
					NETWORK::NETWORK_SESSION_BLOCK_JOIN_REQUESTS(m_on);
				});
			}
		}
	};
}
