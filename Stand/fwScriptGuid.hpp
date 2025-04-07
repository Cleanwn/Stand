#pragma once

#include "gta_extensible.hpp"

namespace rage
{
	struct fwScriptGuid : public fwExtension
	{
		/* 0x08 */ fwExtensibleBase* m_pBase;
	};
	static_assert(sizeof(fwScriptGuid) == 0x10);
}
