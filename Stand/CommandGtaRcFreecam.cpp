#include "CommandGtaRcFreecam.hpp"

#include "ControlMgr.hpp"
#include "NativeTableHooks.hpp"

namespace Stand
{
	CommandGtaRcFreecam::CommandGtaRcFreecam(CommandList* const parent)
		: CommandToggle(parent, LOC("NOGTARCFP"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAME("rcfirstperson")))
	{
	}

	void CommandGtaRcFreecam::onChange(Click& click)
	{
		ControlMgr::rc_freecam = m_on;
		ControlMgr::update();

		if (m_on)
		{
			NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("am_mp_rc_vehicle"), 0x2A2173E46DAECD12, &nullsub<rage::scrNativeCallContext&>);
			NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("am_mp_rc_vehicle"), 0x6D0858B8EDFD2B7D, &nullsub<rage::scrNativeCallContext&>);
			NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("am_mp_rc_vehicle"), 0xB4EC2312F4E5B1F1, &nullsub<rage::scrNativeCallContext&>);
		}
		else
		{
			NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("am_mp_rc_vehicle"), 0x2A2173E46DAECD12);
			NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("am_mp_rc_vehicle"), 0x6D0858B8EDFD2B7D);
			NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("am_mp_rc_vehicle"), 0xB4EC2312F4E5B1F1);
		}
	}
}
