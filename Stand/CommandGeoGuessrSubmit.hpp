#pragma once

#include "CommandAction.hpp"

#include "AbstractEntity.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandGeoGuessrSubmit : public CommandAction
	{
	public:
		explicit CommandGeoGuessrSubmit(CommandList* const parent)
			: CommandAction(parent, LOC("GGSR_SBMT"), {}, LOC("GGSR_SBMT_H"), (CMDFLAGS_ACTION & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_CONCEALED)
		{
		}

		void onClick(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				if (parent->as<CommandGeoGuessr>()->guessed_at == 0)
				{
					auto pos = g_gui.waypoint;
					if (pos.isNull())
					{
						pos = AbstractEntity::getEffectiveUserPos();
					}
					g_tb_screenshot_mode.enable();
					g_tb_screenshot_mode.allow_centred_text = true;
					parent->as<CommandGeoGuessr>()->stopScouting();
					Camgr::startCamCommand();
					parent->as<CommandGeoGuessr>()->guessed_at = get_current_time_millis();
					parent->as<CommandGeoGuessr>()->guess = std::move(pos);
				}
			});
		}
	};
}
