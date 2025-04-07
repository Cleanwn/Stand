#pragma once

namespace rage
{
	struct sysDependency
	{
		typedef bool (Callback)(const sysDependency&);

		union
		{
			float m_AsFloat;
			bool m_AsBool;
			int m_AsInt;
			uint32_t m_AsUInt;
			void* m_AsPtr;
			const void* m_AsConstPtr;
			struct { uint16_t m_Low, m_High; } m_AsShort;
		} m_Params[10];
		/* 0x50 */ sysDependency* m_Parents[2];
		/* 0x60 */ Callback* m_Callback;
		/* 0x68 */ uint32_t m_NumPending;
		/* 0x6C */ uint32_t m_Flags;
		/* 0x70 */ uint32_t m_DataSizes[8];
		/* 0x90 */ uint16_t m_CodeSize;
		/* 0x92 */ uint8_t m_Priority;
		/* 0x93 */ uint8_t m_Chain : 1;
		uint8_t m_Id : 5;
		uint8_t m_JobFailureCount : 2;
		PAD(0x94, 0xA0);
	};
	static_assert(sizeof(sysDependency) == 0xA0);
}
