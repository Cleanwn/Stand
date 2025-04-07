#pragma once

#include "struct_base.hpp"

#include "datBitBuffer.hpp"

namespace rage
{
	// netserialisers.h

	class CSyncDataBase
	{
	public:
		virtual ~CSyncDataBase() = default;
		virtual void _0x08() = 0;
		virtual void _0x10() = 0;
		virtual void _0x18() = 0;
		virtual void _0x20() = 0;
		virtual void _0x28() = 0;
		virtual void _0x30() = 0;
		virtual void SerialiseBool(bool& value, const char* name = nullptr) = 0; // 0x38
		virtual void _0x40() = 0;
		virtual void _0x48() = 0;
		virtual void _0x50() = 0;
		virtual void _0x58() = 0;
		virtual void _0x60() = 0;
		virtual void SerialiseUnsigned(uint32_t& value, const int numBits, const char* name = nullptr) = 0; // 0x68
		virtual void _0x70() = 0;
		virtual void _0x78() = 0;
		virtual void SerialisePackedFloat(float& value, const float maxValue, const int numBits, const char* name = nullptr) = 0; // 0x80
		virtual void _0x88() = 0;
		virtual void _0x90() = 0;
		virtual void _0x98() = 0;
		virtual void _0xA0() = 0;
		virtual void _0xA8() = 0;
		virtual void _0xB0() = 0;
		virtual void SerialiseDataBlock(uint8_t* dataBlock, const int numBits, const char* name = nullptr) = 0; // 0xB8
		virtual void _0xC0() = 0;
		virtual bool GetIsMaximumSizeSerialiser() = 0; // 0xC8

		void SerialiseWeaponType(uint32_t& value)
		{
			SerialiseUnsigned(value, 9);
		}

		void SerialiseAmmoType(uint32_t& value)
		{
			SerialiseUnsigned(value, 9);
		}

		enum Type : uint8_t
		{
			READER = 1,
			WRITER = 2,
			SIZE_CALCULATOR = 3,
			LOGGER = 4,
		};

		/* 0x08 */ Type type;
		PAD(0x09, 0x18) datBitBuffer* bitbuffer;
	};
	static_assert(sizeof(CSyncDataBase) == 0x18 + 8);
	
	class CSyncDataLogger : public CSyncDataBase
	{
	};

	class CSyncDataReader : public CSyncDataBase
	{
	};

	class CSyncDataSizeCalculator : public CSyncDataBase
	{
	};

	class CSyncDataWriter : public CSyncDataBase
	{
	};
}
