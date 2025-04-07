#pragma once

#pragma pack(push, 1)
namespace rage
{
	class RsonWriter
	{
	public:
		/* 0x00 */ uint64_t unk_00 = 0;
		/* 0x08 */ void* data;
		/* 0x10 */ uint32_t unk_10 = 0;
		/* 0x14 */ int32_t size; // size of data in bytes
		/* 0x18 */ BOOL json_mode = FALSE;
		/* 0x1C */ uint32_t unk_added_1_69 = 0;
		/* 0x20 */ uint8_t flags = 0;

		RsonWriter(void* data, int32_t size)
			: data(data), size(size)
		{
		}
	};
	static_assert(sizeof(RsonWriter) == 0x20 + 1);
}
#pragma pack(pop)
