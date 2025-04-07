#pragma once

#pragma pack(push, 1)
namespace rage
{
	class rlTaskBase
	{
	public:
		virtual ~rlTaskBase() = default;
		virtual void _0x08() = 0;
		virtual void _0x10() = 0;
		virtual void Update(unsigned int) = 0;
	};
}
#pragma pack(pop)
