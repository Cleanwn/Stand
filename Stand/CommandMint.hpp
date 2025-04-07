#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandMint : public CommandToggle
	{
	public:
		explicit CommandMint(CommandList* const parent)
			: CommandToggle(parent, LOC("MINT"), { CMDNAME("mint"), CMDNAME("nodecals") }, LOC("MINT_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [=]()
			{
				auto veh = Util::getVehicle();
				if (veh.isValid())
				{
					VEHICLE::SET_VEHICLE_HAS_UNBREAKABLE_LIGHTS(veh, m_on);
					if (!m_on)
					{
						return false;
					}
					veh.wash();
				}
				return m_on;
			});
		}
	};
}
