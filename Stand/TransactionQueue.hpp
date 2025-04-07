#pragma once

#include "gta_fwddecl.hpp"

#include <macros.hpp>

class TransactionQueue
{
public:
	PAD(0x00 + 0, 0x39) bool busy_one;
	PAD(0x39 + 1, 0x48) bool busy_two;
	PAD(0x48 + 1, 0x50) bool busy_three;
};
static_assert(sizeof(TransactionQueue) == 0x50 + 1);
