#include "CommandOnlinePhoneAnims.hpp"

#include "AbstractEntity.hpp"
#include "FiberPool.hpp"
#include "is_session.hpp"
#include "natives.hpp"

namespace Stand
{
	CommandOnlinePhoneAnims::CommandOnlinePhoneAnims(CommandList* const parent)
		: CommandToggle(parent, LOC("PHNANIM"), {}, LOC("PHNANIM_H2"))
	{
	}

	void CommandOnlinePhoneAnims::onEnable(Click& click)
	{
		registerScriptTickEventHandler(click, [this]
		{
			if (!m_on)
			{
				if (is_session_started())
				{
					togglePhoneAnims(false);
				}
				return false;
			}
			if (is_session_started())
			{
				togglePhoneAnims(CAM::GET_CAM_VIEW_MODE_FOR_CONTEXT(CAM::GET_CAM_ACTIVE_VIEW_MODE_CONTEXT()) != 4);
			}
			return true;
		});
	}

	void CommandOnlinePhoneAnims::togglePhoneAnims(bool on)
	{
		PED::SET_PED_CONFIG_FLAG(g_player_ped, 242, !on);
		PED::SET_PED_CONFIG_FLAG(g_player_ped, 243, !on);
		PED::SET_PED_CONFIG_FLAG(g_player_ped, 244, !on);
		if (was_on != on)
		{
			was_on = on;
			if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(ATSTRINGHASH("cellphone_flashhand")) != 0)
			{
				MOBILE::DESTROY_MOBILE_PHONE();
				FiberPool::queueJob([]
				{
					MOBILE::CREATE_MOBILE_PHONE(0);
				});
			}
		}
	}
}
