#pragma once

#include "gta_task.hpp"

enum JumpFlags
{
	JF_SuperJump = (1 << 15),
	JF_BeastJump = (1 << 17),
};

struct CTaskJump : public CTask
{
	INIT_PAD(CTask, 0x218) uint32_t flags;
};
