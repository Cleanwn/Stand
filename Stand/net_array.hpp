#pragma once

#include "struct_base.hpp"

#include "atFixedArray.hpp"
#include "gta_player.hpp"

#pragma pack(push, 1)
namespace rage
{
	class netArrayHandlerBase
	{
	public:
		virtual ~netArrayHandlerBase() = 0;

		PAD(0x08, 0x6C) atFixedArray<netSequence, MAX_PLAYERS> m_LastReceivedSequences;
		PAD(0x6C + sizeof(atFixedArray<netSequence, MAX_PLAYERS>), 0x108);
	};
	static_assert(sizeof(netArrayHandlerBase) == 0x108);

	//template <typename T>
	class netArrayHandler : public netArrayHandlerBase
	{
	public:
		/* 0x108 */ void* m_Array; // T*
	};
	static_assert(sizeof(netArrayHandler) == 0x108 + 8);

	//template <typename T>
	class netPlayerArrayHandler : public netArrayHandler
	{
		INIT_PAD(netArrayHandler, 0x154) bool m_Initialised;
		/* 0x155.0 */ bool m_bHasReceivedAnUpdate : 1;
		/* 0x155.1 */ bool m_bHasHighFrequencyUpdates : 1;
	};
	static_assert(sizeof(netPlayerArrayHandler) == 0x155 + 1);

	class netScriptBroadcastDataHandlerBase : public netPlayerArrayHandler//<CBroadcastDataElement>
	{
	};

	class netHostBroadcastDataHandlerBase : public netScriptBroadcastDataHandlerBase
	{
	};

	class netArrayManager
	{
	public:
		virtual void _0x00() = 0;
		virtual void _0x08() = 0;
		virtual void _0x10() = 0;
		virtual void _0x18() = 0;
		virtual void _0x20() = 0;
		virtual void _0x28() = 0;
		virtual void _0x30() = 0;
		virtual void _0x38() = 0;
		virtual void _0x40() = 0;
		virtual void _0x48() = 0;
		virtual void _0x50() = 0;
		virtual void _0x58() = 0;
		virtual void _0x60() = 0;
		virtual void _0x68() = 0;
	};

	class netArrayManager_Script : public netArrayManager
	{
	public:
		virtual rage::netHostBroadcastDataHandlerBase* GetScriptHostBroadcastDataArrayHandler(const rage::scriptIdBase& scriptId, uint32_t dataId) = 0;
	};
}
#pragma pack(pop)
