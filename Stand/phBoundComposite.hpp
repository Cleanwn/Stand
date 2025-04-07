#pragma once

#include "struct_base.hpp"

namespace rage
{
	class phBoundComposite
	{
		PAD(0x00, 0x70) void** bounds;
	};
}
