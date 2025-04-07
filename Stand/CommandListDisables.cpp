#include "CommandListDisables.hpp"

#include "CommandLambdaToggleScriptTick.hpp"
#include "CommandTogglePointer.hpp"
#include "Hooking.hpp"
#include "natives.hpp"

#include "CommandDisableInputs.hpp"
#include "CommandDisablePhone.hpp"
#include "CommandNoCamShake.hpp"
#include "CommandNoStuntJumps.hpp"
#include "CommandNoCinematicCam.hpp"
#include "CommandDisableVehicleFocus.hpp"
#include "CommandNoIdleCam.hpp"
#include "CommandDisableRestrictedAreas.hpp"
#include "CommandDisableFlightMusic.hpp"
#include "CommandDisableWantedLevelMusic.hpp"
#include "CommandSuppressNumpad7Messages.hpp"

namespace Stand
{
	CommandListDisables::CommandListDisables(CommandList* const parent)
		: CommandList(parent, LOC("DBLES"))
	{
		this->createChild<CommandDisableInputs>();
		this->createChild<CommandDisablePhone>();
		this->createChild<CommandNoCamShake>();
		this->createChild<CommandNoStuntJumps>();
		this->createChild<CommandNoCinematicCam>();
		this->createChild<CommandDisableVehicleFocus>();
		this->createChild<CommandNoIdleCam>();
		this->createChild<CommandLambdaToggleScriptTick>(LOC("DSBLREC"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("recordings")), LOC("DSBLREC_H"), [] { RECORDING::CANCEL_REPLAY_RECORDING(); });
		this->createChild<CommandDisableRestrictedAreas>();
		this->createChild<CommandDisableFlightMusic>();
		this->createChild<CommandDisableWantedLevelMusic>();
		this->createChild<CommandSuppressNumpad7Messages>();
	}
}
