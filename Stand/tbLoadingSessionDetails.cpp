#include "tbLoadingSessionDetails.hpp"

#include "natives.hpp"

namespace Stand
{
	void tbLoadingSessionDetails::onEnable()
	{
		HUD::BEGIN_TEXT_COMMAND_BUSYSPINNER_ON("NT_INV_CONFIG");
		HUD::END_TEXT_COMMAND_BUSYSPINNER_ON(0); // 4 for yellow if we want to indicate that something is different
	}

	void tbLoadingSessionDetails::onDisable()
	{
		HUD::BUSYSPINNER_OFF();
	}
}
