#pragma once

#include "netMessageText.hpp"

#include "rlGamerHandle.hpp"

struct MsgTextMessage : public rage::netMessageText
{
	/* 256 */ rage::rlGamerHandle sender;
};
static_assert(sizeof(MsgTextMessage) == 256 + sizeof(rage::rlGamerHandle));
