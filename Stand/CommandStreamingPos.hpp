#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandStreamingPos : public CommandToggle
	{
	private:
		CommandPositionSelection* const pos_sel;

	public:
		explicit CommandStreamingPos(CommandList* const parent, CommandPositionSelection* const pos_sel)
			: CommandToggle(parent, LOC("LKSTRMFCS"), { CMDNAME("lockstreamingfocus") }), pos_sel(pos_sel)
		{
		}

		void onEnable(Click& click) final
		{
			Camgr::freecam_focus_allowed = false;
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					if (!pos_sel->is_being_changed)
					{
						Camgr::lock_focus(pos_sel->pos);
					}
					return true;
				}
				Camgr::freecam_focus_allowed = true;
				Camgr::unlock_focus();
				return false;
			});
		}
	};
}
