#pragma once

#include "CommandReadonlyLink.hpp"

#include "Gui.hpp"
#include "RelayCon.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandWebOpen : public CommandReadonlyLink
	{
	public:
		explicit CommandWebOpen(CommandList* const parent)
			: CommandReadonlyLink(parent, LOC("WEB"), {}, NOLABEL, { CMDNAME("webinterface") })
		{
		}
		
		void onClick(Click& click) final
		{
			if (click.isWeb())
			{
				Util::toast_web_uwotm8();
				return;
			}
			if (!click.isRegularEdition())
			{
				return;
			}
			g_relay.setServerAndInit();
			g_gui.shellExecute(CommandWeb::getUri().c_str());
		}
	};
}
