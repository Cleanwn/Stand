#include "timescale.hpp"

#include <soup/Pointer.hpp>

#include "natives.hpp"
#include "pointers.hpp"

namespace Stand
{
	void setTimescale(float timescale)
	{
		g_timescale = timescale;
		MISC::SET_TIME_SCALE(timescale);

		// Patch to allow us to go above 1.0
		float* pDesiredAuthoritativeTimeScale;
		if (timescale == 1.0f)
		{
			pDesiredAuthoritativeTimeScale = pointers::rage_fwTimer_sm_fTimeScale;
		}
		else
		{
			pDesiredAuthoritativeTimeScale = &(*pointers::rage_fwTimer_sm_aTimeScales)[1];
		}
		*pointers::authoritative_time_scale_offset = soup::Pointer(pointers::authoritative_time_scale_offset).unrip(pDesiredAuthoritativeTimeScale);
	}
}
