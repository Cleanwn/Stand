#pragma once

#include "CommandToggle.hpp"
#include "ScriptGlobal.hpp"

namespace Stand {
	class CommandDisableVehicleFocus : public CommandToggle
	{
	public:
		explicit CommandDisableVehicleFocus(CommandList* const parent)
			: CommandToggle(parent, LOC("NVEHFCSCAM"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("focuscamera")), LOC("NVEHFCSCAM_H")) {}

		void onChange(Click& click) final
		{
			if(m_on)
			{
				registerScriptTickEventHandler(click, [=]()
				{
					if(m_on)
					{
						*ScriptGlobal(GLOBAL_DISABLE_VEHICLE_FOCUS).as<int*>() = 1;
						return true;
					}
					else
					{
						*ScriptGlobal(GLOBAL_DISABLE_VEHICLE_FOCUS).as<int*>() = 0;
						return false;
					}
				});
			}
		}
	};
}
