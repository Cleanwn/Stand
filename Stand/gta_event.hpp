#pragma once

#include "fwRefAwareBase.hpp"

namespace rage
{
	class fwEvent : public fwRefAwareBase
	{
	public:
		// 0x00 - rtti shim 1
		// 0x08 - rtti shim 2
		// 0x10 - rtti shim 3
		// 0x18 - rtti shim 4
		// 0x20 - rtti shim 5
		// 0x28 - rtti shim 6
		/* 0x30 */ virtual ~fwEvent() = default;
		/* 0x38 */ virtual void unk_0x08() = 0;
		/* 0x40 */ virtual bool equals(const fwEvent& b) = 0;
		/* 0x48 */ virtual int GetEventType() = 0; // 2802: 0x18 -> 0x48 (+0x30)
		/* 0x50 */ virtual void unk_0x20() = 0;
		/* 0x58 */ virtual void unk_0x28() = 0;
		/* 0x60 */ virtual void RetrieveData(void* args, unsigned int args_size_bytes) = 0; // 2802: 0x30 -> 0x60 (+0x30)
	};
}

class CEventNetwork : public rage::fwEvent
{
};

class CEventGroupScriptNetwork
{
public:
	virtual ~CEventGroupScriptNetwork() = default;
	virtual void __fastcall dispatchEvent(rage::fwEvent* event) = 0;
};
