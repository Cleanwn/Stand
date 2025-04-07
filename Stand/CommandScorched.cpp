#include "CommandScorched.hpp"

#include "AbstractEntity.hpp"
#include "gta_entity.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandScorched::CommandScorched(CommandList* const parent)
		: CommandToggle(parent, LOC("SCRCHD"), { CMDNAME("scorched"), CMDNAME("bbq"), CMDNAME("barbeque") })
	{
	}

	void CommandScorched::onChange(Click& click)
	{
		onChangeToggleScriptTickEventHandler(click, []
		{
			if (auto e = Util::getVehicle(); e.isValid())
			{
				if (auto v = e.getPointer())
				{
					v->m_nPhysicalFlags.bRenderScorched = true;
				}
			}
		});
		if (!m_on)
		{
			if (auto e = Util::getVehicle(); e.isValid())
			{
				if (auto v = e.getPointer())
				{
					v->m_nPhysicalFlags.bRenderScorched = false;
				}
			}
		}
	}
}
