#if false
#include "CommandPersonalVehicleInsurance.hpp"

#include "CommandPersonalVehicle.hpp"
#include "PersonalVehicle.hpp"

namespace Stand
{
	CommandPersonalVehicleInsurance::CommandPersonalVehicleInsurance(CommandList* parent)
		: CommandToggle(parent, LOC("INSUR"), {}, NOLABEL, false, (CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_TEMPORARY)
	{
	}

	void CommandPersonalVehicleInsurance::onChange(Click& click)
	{
		if (!click.isBulk())
		{
			PersonalVehicle{ ((CommandPersonalVehicle*)parent)->i }.setInsuranceFlag(GLOBAL_PVS_INSURANCE_INSURED, m_on);
			PersonalVehicle::saveStats();
			((CommandPersonalVehicle*)parent)->updateHelpText();
		}
	}

	void CommandPersonalVehicleInsurance::onTickInGameViewport()
	{
		setOnIndication(PersonalVehicle{ ((CommandPersonalVehicle*)parent)->i }.getInsuranceFlag(GLOBAL_PVS_INSURANCE_INSURED));
	}

	void CommandPersonalVehicleInsurance::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
#endif
