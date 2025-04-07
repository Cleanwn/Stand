#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandVehicleNoCollision : public CommandToggle
	{
	public:
		explicit CommandVehicleNoCollision(CommandList* const parent)
			: CommandToggle(parent, LOC("GHOSTRIDE"), { CMDNAME("vehiclenocollision"), CMDNAME("vehnocollision"), CMDNAME("nocollisionvehicle") }, LOC("GHOSTRIDE_H"))
		{
		}

		void onClick(Click& click) final
		{
			if (!m_on)
			{
				AbstractEntity veh = Util::getCurrentlyDrivingVehicle(click);
				if (veh.isValid())
				{
					ensureScriptThread(click, [this, veh{ std::move(veh) }](Click& click) mutable
					{
						m_on = true;
						updateState(click);
						registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [this, veh{ std::move(veh) }]() mutable
						{
							if (m_on && veh == Util::getVehicle(false))
							{
								ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(veh, false, false);
								return true;
							}
							if (veh.exists())
							{
								ENTITY::SET_ENTITY_COLLISION(veh, true, false);
							}
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
				else if (click.isWeb())
				{
					updateWebState();
				}
			}
			else
			{
				m_on = false;
				updateState(click);
			}
		}
	};
}
