#pragma once

#include "struct_base.hpp"

namespace rage
{
	struct rlScAccountInfo
	{
		/* 0x000 */ int32_t age;
		/* 0x004 */ char avatar_url[160];
		/* 0x0A4 */ char country_code[10];
		PAD(0x0AE, 0x0C7) char email_address[102];
		/* 0x12D */ char language_code[10];
		/* 0x137 */ char nickname[17];
		PAD(0x148, 0x180) int64_t rockstar_id;
	};
	static_assert(sizeof(rlScAccountInfo) == 0x188);
}
