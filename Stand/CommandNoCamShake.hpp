#pragma once

#include "CommandToggle.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandNoCamShake : public CommandToggle
	{
	public:
		explicit CommandNoCamShake(CommandList* const parent)
			: CommandToggle(parent, LOC("NCAMSHK"), combineCommandNames(CMDNAMES_OBF("no", "block", "disable"), CMDNAMES_OBF("camshake", "camerashake")))
		{
		}

		void onChange(Click& click) final
		{
			if (g_hooking.camFactory_CreateObject_hook.isHooked())
			{
				g_hooking.disable_cam_shake = m_on;
			}
			else // Hook is missing?
			{
				if (m_on) // User wants to turn it on?
				{
					click.setResponse(LIT(LANG_FMT("HKFAIL", LANG_GET("NCAMSHK"))));
				}
			}
		}
	};
}
