#pragma once

#include "CommandListViewport.hpp"
#include "CommandListConcealer.hpp"

namespace Stand
{
	class CommandListVehicleExtras : public CommandListViewport<CommandListConcealer>
	{
	public:
		using CommandListViewport::CommandListViewport;

		void onClick(Click& click) final
		{
			if (auto veh = Util::getVehicle(); veh.isValid())
			{
				for (auto i = 1; i != 15; ++i)
				{
					if (VEHICLE::DOES_EXTRA_EXIST(veh, i))
					{
						return CommandListConcealer::onClick(click);
					}
				}
			}
			onClickImplUnavailable(click);
			click.setResponse(LOC("XTRAS_N"));
		}
	};
}
