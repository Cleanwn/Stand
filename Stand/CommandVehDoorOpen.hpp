#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandVehDoorOpen : public CommandToggle
	{
	private:
		const int door_id;

	public:
		explicit CommandVehDoorOpen(CommandList* parent, int door_id, Label&& menu_name)
			: CommandToggle(parent, std::move(menu_name), {}, NOLABEL, false), door_id(door_id)
		{
			if (door_id == -1)
			{
				command_names = { CMDNAME("openvehdoors"), CMDNAME("openvehicledoors") };
			}
		}

	private:
		[[nodiscard]] std::vector<int> getDoors() const
		{
			if (door_id == -1)
			{
				return { 0, 1, 2, 3, 4, 5, 6, 7 };
			}
			return { door_id };
		}

	public:
		void onTickInGameViewport() final
		{
			if (!job_queued)
			{
				Vehicle veh = Util::getVehicle();
				if (veh == 0)
				{
					setOnIndication(false);
				}
				else
				{
					bool is_on = false;
					auto doors = getDoors();
					for (auto door : doors)
					{
						if (VEHICLE::GET_VEHICLE_DOOR_ANGLE_RATIO(veh, door) > 0.5f)
						{
							is_on = true;
							break;
						}
					}
					setOnIndication(is_on);
				}
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void onChange(Click& click) final
		{
			const bool on = m_on;
			ensureScriptThread(click, [this, click, on]
			{
				Click click_(click);
				Vehicle veh = Util::getVehicle(click_);
				if (veh != 0)
				{
					auto doors = getDoors();
					for (auto door : doors)
					{
						if (on)
						{
							VEHICLE::SET_VEHICLE_DOOR_OPEN(veh, door, false, door_id != -1);
						}
						else
						{
							VEHICLE::SET_VEHICLE_DOOR_SHUT(veh, door, door_id != -1);
						}
					}
				}
			});
		}
	};
}
