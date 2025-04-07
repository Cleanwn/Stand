#pragma once

// As of 3095, only seems to work via fake friends.
#define CAN_SPECTATE false

#define I_CAN_SPY false

#ifdef STAND_DEBUG
#define SPY_CAN_KICK false // no longer works as of 1.69
#else
#define SPY_CAN_KICK false
#endif
