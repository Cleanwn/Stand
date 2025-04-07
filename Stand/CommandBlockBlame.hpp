#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandBlockBlame : public CommandToggle
	{
	public:
		explicit CommandBlockBlame(CommandList* const parent)
			: CommandToggle(parent, LOC("PTX_PSV"), combineCommandNames(std::vector<CommandName>{ CMDNAME("no"), CMDNAME("block") }, std::vector<CommandName>{ CMDNAME("blame"), CMDNAME("blaime"), CMDNAME("blaming") }), LOC("PTX_PSV_H2"))
		{
		}

		void onEnable(Click& click) final
		{
			if (click.isAuto())
			{
				return onEnableImpl(click);
			}
			setOnIndication(false);
			showWarning(click, LANG_GET_W("PTX_PSV_W"), 0, [this](ThreadContext thread_context)
			{
				setOnIndication(true);
				Click click{ CLICK_AUTO, thread_context };
				onEnableImpl(click);
			});
		}

	private:
		void onEnableImpl(Click& click)
		{
			registerScriptTickEventHandler(click, [this]
			{
				NETWORK::NETWORK_SET_PLAYER_IS_PASSIVE(m_on);
				NETWORK::NETWORK_SET_FRIENDLY_FIRE_OPTION(!m_on);
				return m_on;
			});
		}
	};
}
