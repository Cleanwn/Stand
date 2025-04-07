#pragma once

#include "CommandToggle.hpp"

#include "SharedNativeHooks.hpp"

namespace Stand
{
	class CommandHideTyping : public CommandToggle
	{
	public:
		explicit CommandHideTyping(CommandList* const parent)
			: CommandToggle(parent, LOC("NTPNGIDCTR"), CMDNAMES("hidetyping", "notypingindicator", "disabletypingindicator"), LOC("NTPNGIDCTR_H"))
		{
		}

		void onChange(Click& click) final
		{
			SharedNativeHooks::block_outgoing_typing_indicator_events = m_on;
		}
	};
}
