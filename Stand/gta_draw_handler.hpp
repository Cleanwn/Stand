#pragma once

#include "pgBase.hpp"

namespace rage
{
	class rmcDrawableBase : public pgBase
	{
	};

	class rmcDrawable : public rmcDrawableBase
	{
	};

	class fragDrawable : public rmcDrawable
	{
	};

	class fwDrawData
	{
	public:
		virtual ~fwDrawData() = default;

		/* 0x08 */ fragDrawable* drawable;
		PAD(0x10, 0x20) fwCustomShaderEffect* m_pShaderEffect;
	};
	static_assert(sizeof(fwDrawData) == 0x20 + 8);
}

class CEntityDrawHandler : public rage::fwDrawData
{
};

class CDynamicEntityDrawHandler : public CEntityDrawHandler
{
};
