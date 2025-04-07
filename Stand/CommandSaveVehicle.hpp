#pragma once

#include "CommandActionName.hpp"

#include "ColonsVehicleCustomisationIo.hpp"
#include "Util.hpp"
#include "VehicleEntityCustomisationIo.hpp"

namespace Stand
{
	class CommandSaveVehicle : public CommandActionName
	{
	public:
		explicit CommandSaveVehicle(CommandList* parent)
			: CommandActionName(parent, LOC("SVEVEH"), CMDNAMES("savevehicle"))
		{
		}

		void onClick(Click& click) final
		{
			if (Util::getVehicle(click).isValid())
			{
				CommandAction::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!onCommandValidateFileName(click, args))
			{
				return;
			}
			auto veh = Util::getVehicle(click);
			if (!veh.isValid())
			{
				args.clear();
				return;
			}
			VehicleEntityCustomisationIo veh_cio{ veh };
			SOUP_IF_LIKELY (ColonsVehicleCustomisationIo::handleSaveCommand(click, args, veh_cio, &veh))
			{
				((CommandListFolder*)parent)->refresh();
			}
		}
	};
}
