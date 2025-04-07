#include "CommandSavePv.hpp"

#include "ColonsVehicleCustomisationIo.hpp"
#include "CommandPersonalVehicle.hpp"
#include "PersonalVehicleCustomisationIo.hpp"

namespace Stand
{
	CommandSavePv::CommandSavePv(CommandList* parent)
		: CommandActionName(parent, LOC("SVEVEH"), parent->getSuffixedCommandNames(CMDNAME("save")), LOC("PVSVE_H"), CMDFLAGS_ACTION_NAME | CMDFLAG_TEMPORARY)
	{
	}

	void CommandSavePv::onCommand(Click& click, std::wstring& args)
	{
		if (onCommandValidateFileName(click, args))
		{
			PersonalVehicleCustomisationIo veh_cio{ parent->as<CommandPersonalVehicle>()->i };
			ColonsVehicleCustomisationIo::handleSaveCommand(click, args, veh_cio);
		}
	}
}
