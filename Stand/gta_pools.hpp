#pragma once

#include <cstdint>

namespace Stand
{
	template <typename T>
	struct EntityPool
	{
		void iterate(const std::function<void(rage::fwEntity*)>&& func)
		{
			for (int i = 0; i < ((T*)this)->size; i++)
			{
				if (((T*)this)->isValid(i))
				{
					func(((T*)this)->getAddress(i));
				}
			}
		}
	};
}

namespace rage
{
	class GenericPool : public Stand::EntityPool<GenericPool>
	{
	public:
		uint64_t poolStartAddress;
		uint8_t* byteArray;
		int  size;
		int  itemSize;

		bool isValid(int i)
		{
			return mask(i) != 0;
		}

		rage::fwEntity* getAddress(int i)
		{
			return (rage::fwEntity*)(mask(i) & (poolStartAddress + i * itemSize));
		}

	private:
		long long mask(int i)
		{
			long long num1 = byteArray[i] & 0x80; // check for high bit.
			return ~((num1 | -num1) >> 63);
		}
	};

	class VehiclePool : public Stand::EntityPool<GenericPool>
	{
	public:
		rage::fwEntity** listAddress;
		int  size;
		char _padding2[36];
		uint32_t* bitArray;
		char _padding3[40];
		int  itemCount;

		bool isValid(int i)
		{
			return (bitArray[i >> 5] >> (i & 0x1F)) & 1;
		}

		rage::fwEntity* getAddress(int i)
		{
			return listAddress[i];
		}
	};
}
