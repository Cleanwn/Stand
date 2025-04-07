#pragma once

#include "CommandToggleNoCorrelation.hpp"

#include "Auth.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandStandUserIdentification : public CommandToggleNoCorrelation
	{
	public:
		explicit CommandStandUserIdentification(CommandList* const parent)
			: CommandToggleNoCorrelation(parent, LOC("SUI"), {}, LOC("MDDRDT_SUI_H"), true)
		{
		}

		void onEnable(Click& click) final
		{
			if (g_gui.canUpdateThemeSettingWithDefaultDependency())
			{
				g_auth.stand_user_identification = true;
			}
		}

		void onDisable(Click& click) final
		{
			g_auth.stand_user_identification = false;
		}
	};
}
