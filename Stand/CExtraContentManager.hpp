#pragma once

#include "struct_base.hpp"

#include "atArray.hpp"
#include "atString.hpp"

struct SSetupData
{
	/* 0x00 */ rage::atString m_deviceName;
	/* 0x10 */ rage::atString m_datFile;
	/* 0x20 */ rage::atString m_timeStamp;
	/* 0x30 */ hash_t m_nameHash;
	PAD(0x34, 0x88);
};
static_assert(sizeof(SSetupData) == 0x88);

class CMountableContent
{
	PAD(0, 0x30) SSetupData m_setupData;
	/* 0xB8 */ rage::atString m_filename;
	PAD(0xB8 + 0x10, 0xF0);
};
static_assert(sizeof(CMountableContent) == 0xF0);

class CExtraContentManager
{
	PAD(0, 0x28) rage::atArray<CMountableContent> m_content;
	PAD(0x28 + sizeof(rage::atArray<CMountableContent>), 0x182) bool m_enumerating : 1;
	bool m_enumerateOnUpdate : 1;
	bool m_everHadBadPackOrder : 1;
	bool m_specialTriggerTunablesTested : 1;
};
