#pragma once

#pragma pack(push, 1)
namespace rage
{
	class scriptResource
	{
	public:
		virtual ~scriptResource() = default;
	};

	class scriptResourceEntry
	{
	public:
		scriptResource* m_data;      // 0x00
		std::uint32_t m_unk;         // 0x04
		char m_padding[0x0C];        // 0x0C
		scriptResourceEntry* m_next; // 0x18
	};
}

class CGameScriptResource : public rage::scriptResource
{
};

class CScriptResource_Camera : public CGameScriptResource
{
	 INIT_PAD(CGameScriptResource, 0x10) int cam_handle;
};
static_assert(sizeof(CScriptResource_Camera) == 0x10 + 4);
#pragma pack(pop)
