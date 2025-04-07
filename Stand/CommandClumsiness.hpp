#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandClumsiness : public CommandToggle
	{
	private:
		CommandToggleNoCorrelation* const grace;
		time_t reenable_timer = 0;

	public:
		explicit CommandClumsiness(CommandList* const parent, CommandToggleNoCorrelation* grace)
			: CommandToggle(parent, LOC("CLUMSY"), { CMDNAME("clumsiness"), CMDNAME("clumsyness") }, LOC("CLUMSY_H")), grace(grace)
		{
		}

		void onEnable(Click& click) final
		{
			if (grace->m_on)
			{
				Util::toast(LANG_FMT("NO_GRACE", FMT_ARG("feature_name", this->menu_name.getLocalisedUtf8())));
			}
			registerScriptTickEventHandler(click, [=]()
			{
				if (!m_on)
				{
					PED::SET_PED_RAGDOLL_ON_COLLISION(g_player_ped, false);
					return false;
				}
				if (PED::IS_PED_RAGDOLL(g_player_ped))
				{
					reenable_timer = get_current_time_millis();
				}
				else if (GET_MILLIS_SINCE(reenable_timer) > 2000)
				{
					PED::SET_PED_RAGDOLL_ON_COLLISION(g_player_ped, true);
				}
				return true;
			});
		}
	};
}
