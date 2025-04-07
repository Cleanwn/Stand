#pragma once

namespace Stand
{
	enum eAircraftSection
	{
		INVALID_SECTION = -1,

		WING_L,
		WING_R,
		TAIL,
		ENGINE_L,
		ENGINE_R,
		ELEVATOR_L,
		ELEVATOR_R,
		AILERON_L,
		AILERON_R,
		RUDDER,
		RUDDER_2,
		AIRBRAKE_L,
		AIRBRAKE_R,

		NUM_DAMAGE_SECTIONS
	};
}