#pragma once

#include "CommandToggle.hpp"

#include "Camgr.hpp"
#include "Gui.hpp"

namespace Stand
{
	class Command2ndPerson : public CommandToggle
	{
	public:
		explicit Command2ndPerson(CommandList* const parent)
			: CommandToggle(parent, LOC("2PRSN"), CMDNAMES("2ndperson", "secondperson"))
		{
		}

		void onEnable(Click& click) final
		{
			ensureYieldableScriptThread(click, [this]
			{
				Camgr::startCamCommand();
				g_gui.second_person_cam = true;
				registerScriptTickEventHandler(TC_SCRIPT_YIELDABLE, [this]()
				{
					if (m_on && Camgr::canCamCommandRun())
					{
						return true;
					}
					g_gui.second_person_cam = false;
					Camgr::unlock_pos();
					Camgr::unlock_rot();
					Camgr::endCamCommand();
					if (m_on)
					{
						m_on = false;
						Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
						updateState(click);
					}
					return false;
				});
			});
		}
	};
}
