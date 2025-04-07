#pragma once

#include "inlist.hpp"

namespace rage
{
	class datGrowBuffer
	{
	public:
		uint8_t* m_Buf;
		uint32_t m_Capacity;
		uint32_t m_Len;
		uint32_t m_Flags;
	};

	class netHttpRequest
	{
	public:
		struct Chunk
		{
			datGrowBuffer m_Data;
			PAD(sizeof(datGrowBuffer), 0xA0) inlist_node<Chunk> m_ListLink;
		};

		typedef inlist<Chunk, &Chunk::m_ListLink> ChunkList;

		/* 0x00 */ Chunk* m_CurChunk;
		/* 0x08 */ ChunkList m_QueuedChunks;
		/* 0x20 */ Chunk* m_InFlightChunk;
		PAD(0x28, 0x58) char* ResponseHeaders;
		char pad_0060[40]; //0x0060
		char* RequestHeaders; //0x0088
		char pad_0090[1112]; //0x0090
		struct Chunk* ResponseData; //0x04E8
		char pad_04F0[4]; //0x04F0
		uint16_t ContentLength; //0x04F4
		char pad_04F6[18]; //0x04F6
		char* Scheme; //0x0508
		const char* Host; //0x0510
		char* Path; //0x0518
		char pad_0520[72]; //0x0520
		char* UserAgent; //0x0568
		char pad_0570[3032]; //0x0570
		char* URLRequest; //0x1148
		char pad_1150[1432]; //0x1150
	};
	static_assert(sizeof(netHttpRequest) == 0x16E8);
}
