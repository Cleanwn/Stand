#pragma once

#include "struct_base.hpp"

#pragma pack(push, 1)
class CloudEvent
{
public:
	struct sRequestFinishedEvent
	{
		/* 0 */ uint32_t request_id;
		PAD(4, 8) bool success;
		PAD(8 + 1, 16) char* data;
		/* 24 */ unsigned int size;
	};
	static_assert(sizeof(sRequestFinishedEvent) == 24 + 4);

	/* 0x00 */ unsigned int type;
	PAD(0x04, 0x08) sRequestFinishedEvent data; // actually a union
};
static_assert(sizeof(CloudEvent) == 0x08 + sizeof(CloudEvent::sRequestFinishedEvent));
#pragma pack(pop)
