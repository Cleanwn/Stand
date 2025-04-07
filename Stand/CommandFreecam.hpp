#pragma once

#include "CommandToggle.hpp"

#include "Camgr.hpp"
#include "tbFreecam.hpp"
#include "TpUtil.hpp"
#include "Vector2Plus.hpp"

namespace Stand
{
	class CommandFreecam : public CommandToggle
	{
	private:
		CommandToggleNoCorrelation* const tp_on_disable;

	public:
		explicit CommandFreecam(CommandList* const parent, CommandToggleNoCorrelation* const tp_on_disable)
			: CommandToggle(parent, LOC("FC"), CMDNAMES("freecam"), LOC("FC_H"), false, CMDFLAGS_TOGGLE | CMDFLAG_TUTORIAL_CTX_POINT),
			tp_on_disable(tp_on_disable)
		{
		}

		void onEnable(Click& click) final
		{
			ensureYieldableScriptThread(click, [=]
			{
				Camgr::startCamCommand();
				g_tb_freecam.enable();
				registerScriptTickEventHandler(TC_SCRIPT_YIELDABLE, [this]()
				{
					if (m_on && Camgr::canCamCommandRun())
					{
						return true;
					}
					g_tb_freecam.disable();
					if (tp_on_disable->m_on)
					{
						TpUtil::teleport(g_tb_freecam.pos);
					}
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
