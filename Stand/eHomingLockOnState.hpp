#pragma once

enum eHomingLockOnState : int32_t
{
	HLOnS_NONE = 0x0,
	HLOnS_ACQUIRING = 0x1,
	HLOnS_ACQUIRED = 0x2,
};